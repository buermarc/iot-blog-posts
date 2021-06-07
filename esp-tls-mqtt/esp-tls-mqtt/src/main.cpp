/**
   httpUpdate.ino

    Created on: 27.11.2015

*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include "secret.h"

#define MANUAL_SIGNING 1

const char cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDYDCCAkgCFAby5WtzERmyqC5tDuNvHyzZnwV1MA0GCSqGSIb3DQEBCwUAMHwx
CzAJBgNVBAYTAkRFMQswCQYDVQQIDAJCVzELMAkGA1UEBwwCS0ExDDAKBgNVBAoM
A2lvdDEMMAoGA1UECwwDaW90MRkwFwYDVQQDDBBteWRvbWFpbm5hbWUueHl6MRww
GgYJKoZIhvcNAQkBFg15b3VyQG1haWwuY29tMB4XDTIxMDYwNzE2NDYyNVoXDTIy
MDYwMjE2NDYyNVowXTELMAkGA1UEBhMCREUxCzAJBgNVBAgMAkJXMQswCQYDVQQH
DAJLQTEMMAoGA1UECgwDaW90MQwwCgYDVQQLDANpb3QxGDAWBgNVBAMMDzE5Mi4x
NjguMTc4LjEyMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMV2CyW0
lxuv+CZFbKUp2Qzs0rjafUminGbeFw54+IRHhJJSm4ddLmObm29dUYErtUOLDK+i
j7reWbA4xKyyP/3mYyRqVQnmLzqe7rCPe1ah3R/JhF1O95P9jKDlZfxAYw6jhAEG
0EYCEhoOMfnNRjcXE/rgJMAYzyXxqudISFKBGA8ci0nyFOSlQd5WL9zUBGt1L967
zLhTdOOGsVuMZ3sV2fvYRflMFjwMVoO+B81h66THJXo28FQ3eQju5dL3c+uxu2Cm
l33/wAJI6Sus8iM1N2270FDhiRkkdiNiQnxGMVuTp+I7hhF8lAtPGKxZwR4DU/zR
Gw8AYejc+XV4OmMCAwEAATANBgkqhkiG9w0BAQsFAAOCAQEAC1TT5P6sLg2CmGlZ
8IG3AIBg/8Tnhon7nONiF/0pi/8uod093WHKGdHUW4KD+dyMk5vo32qwx7cT2oSz
LvtDibhF01Nivk633g2WHk31KKcvRZ9t72pxIdU/ZibnYNR4034m/3F050t6peI9
MfhIF3+QVkhjkyoLdX2gz1caeVo2u9aR0t7gnMbJByNYbvjXJu0qE94QfHnSSxMd
q6jVoZBd5eqYJKqZ2I+230CNbqJKsy0SEa0XnypkHK0MFOePjow70izN/345UreI
47CM224yaMJDMmjsbJRaPoOTeFSnkTl2DC2PDII60QSLk+1zRWwLhSpgqKz5ERw1
VhxzNA==
-----END CERTIFICATE-----
)EOF";

const char private_key[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAxXYLJbSXG6/4JkVspSnZDOzSuNp9SaKcZt4XDnj4hEeEklKb
h10uY5ubb11RgSu1Q4sMr6KPut5ZsDjErLI//eZjJGpVCeYvOp7usI97VqHdH8mE
XU73k/2MoOVl/EBjDqOEAQbQRgISGg4x+c1GNxcT+uAkwBjPJfGq50hIUoEYDxyL
SfIU5KVB3lYv3NQEa3Uv3rvMuFN044axW4xnexXZ+9hF+UwWPAxWg74HzWHrpMcl
ejbwVDd5CO7l0vdz67G7YKaXff/AAkjpK6zyIzU3bbvQUOGJGSR2I2JCfEYxW5On
4juGEXyUC08YrFnBHgNT/NEbDwBh6Nz5dXg6YwIDAQABAoIBAQCc3go9ChqBhGo+
/SgyjexAuGMvN2QQ+34EuqrWdIP5kldfZwDnqL8Roqz468m0NGTsI6sQXLSSX9Ig
jPixuWnc6woYA+FK2/LpPxmgalwxlqV0e0MMXY+Rofv2VkeO/hezqXNU3XTYKQz4
Zg6NxOXjHhJhW2/Wq97k5fg/hMzvvPlT/sCvPLa2yDLzK5a14BO9TJRIoFwnFObH
I58OaoINqiP1ZUag0wHys8QiWQ9DV1aQfQZ4nD1U55nxj3ebZ4JEJwFPvbQfpmrc
OULfLgWXvBITBgHVucwOeusqFC0t6sNZeconyVAP4AKyqLRyphlO3TaPJIL4yqFu
AcyZgWEBAoGBAOhR3JC45dweWtfOnG10pvJPO7E/XDStcnXPQ1WpiKAxvVdkUsy7
0l8z8tnFixu3bexn11m8HnZwPTTf/eGVltHVOwhsRa2m8VLcd8uRGn0BlW0j9caT
wBh8H3SAZKvN8Nim9RYZFg9y1xV/WrGk7ZXQ02p/TGAeSSuvCHREoaZFAoGBANmW
lQAphxQsuj0VZbBOGiRgCePZLL9/3Azx6StNRF2/IxVlmQD/q5RjQLYQqVD2IxK1
1qpHcb126dRSRuG6UEffMKdbSXM3EENH8L8TpQAyIx/3hWySgvwjM0wJRJgWOHeu
fz1w8AGWsz/U6Bg42wnXYevKK205qCKYEFZ7ghyHAoGASSfF+biPgTH5zy0bawgp
rfGvTVAzW88mVNywSmA5SqB/C+7md9vJEfuMxHCFLnQjZCcK1BH90bPkQisqigeN
14N6tFtL0bPZGAuemXaWzbha8mQ5d499FPi4+vmXOdZ+uepREOVTYgf6nKVezMOv
oNaCTG3LghTnW58hXWNjN7UCgYAm6HVWZRahdeoLmDLp1t132bCLDL+isrNfoTZn
ptZtyQr1/YfhlNZSn3jn1YzhTFIzO07afhIJpiTj8Z55KL7IS1HA62Lz9kmzLj8P
e+zKXyzGv5UdOAmyGn1GwHWCmJ6aUBqymupf7lm5NVIXWrtYRCpfZnRjgKbfIL/z
Jvy6KwKBgQCB7qtuxIxf6hjINBnJnZ8J4HBr4eteuCUZcnyMpD+cRY1UlkEiiWM0
Ma5jbaqddUFkABTraAf8SMxZ3TLFhte29wp8yCVibtv/5rbN8VPCevRy/5BOX5gD
/X36xOFF78qAL42jtYpK90KOKAyfoJ5H7HIRFcoMwTeWyluSaDjIlw==
-----END RSA PRIVATE KEY-----
)EOF";

const char pubkey[] PROGMEM = R"EOF(
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEArSf4W99aciiCoDH427w5
FE95jL7R/2tC4pYOyZWq3yTVl0Kq6y33L9GgLS6kCILLBi0KSGykOQX3kEzOnZa2
nesjLZXwTxWeRlq1f0OcRMXXNjbVg+kDepKoinW3ch1fD29sLpzUgtNwIt7fAahe
eZGIsytNMnLPRUf0mjKbWe9xgyT88EETPLzsJ9Lw+CJUBxxenmzh5XbU8H/VwUJq
Kjd2ta8jnK6htBPPMvdYpTpCqE+QY4Tp8VmKv2hnCrb8XlIyEfD5y+y5qrIF4Bg1
vRKQD82QmNZon2ASuqPUz45ZQwVqTQSt8Pg4QI7sViO5LTmJuqAAQecHVEZ8ae3J
BwIDAQAB
-----END PUBLIC KEY-----
)EOF";
#if MANUAL_SIGNING
BearSSL::PublicKey *signPubKey = nullptr;
BearSSL::HashSHA256 *hash;
BearSSL::SigningVerifier *sign;
#endif

BearSSL::X509List *list = nullptr;
BearSSL::PrivateKey *key = nullptr;

#include <time.h>

#include <FS.h>
#include <LittleFS.h>

#include <CertStoreBearSSL.h>
BearSSL::CertStore certStore;

#include <base64.h>

#include <PubSubClient.h>

BearSSL::WiFiClientSecure client;
PubSubClient pubsub_client(client);


const String version = "VERSION_0.0.1";

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // UTC

  Serial.print(F("Waiting for NTP time sync: "));
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    Serial.print(F("."));
    now = time(nullptr);
  }

  Serial.println(F(""));
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

    setClock();

    bool mfln = client.probeMaxFragmentLength("server", 443, 1024);  // server must be the same as in ESPhttpUpdate.update()
    Serial.printf("MFLN supported: %s\n", mfln ? "yes" : "no");
    if (mfln) {
      client.setBufferSizes(1024, 1024);
    }

    Serial.print("Local WiFi: ");
    Serial.println(WiFi.localIP());

    #if MANUAL_SIGNING
    // Ensure all updates are signed appropriately.  W/o this call, all will be accepted.
    Update.installSignature(hash, sign);
    #endif


    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

    // Add optional callback notifiers
    
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    String header = base64::encode(version + "::" + SECRET);
    //t_httpUpdate_return ret = ESPhttpUpdate.update(client, "https://192.168.178.123/firmware.bin.signed", );
    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "192.168.178.123", 8080, "/firmware.bin.signed");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }

}


void reconnect() {
  // Loop until we're reconnected
  while (!pubsub_client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (pubsub_client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      pubsub_client.publish("outTopic","hello world");
      // ... and resubscribe
      pubsub_client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubsub_client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

}


ESP8266WiFiMulti WiFiMulti;

void mqtt_setup() {
  pubsub_client.setServer("192.168.178.123", 8883);
  pubsub_client.setCallback(callback);
  delay(1500);
}

void client_tls_setup() {
  list = new BearSSL::X509List(cert);
  key = new BearSSL::PrivateKey(private_key);
  client.setClientRSACert(list, key);
  delay(1500);
}

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(APSSID, APPSK);

  #if MANUAL_SIGNING
  signPubKey = new BearSSL::PublicKey(pubkey);
  hash = new BearSSL::HashSHA256();
  sign = new BearSSL::SigningVerifier(signPubKey);
  #endif

  LittleFS.begin();

  Serial.print("Exists"); 
  Serial.println(LittleFS.exists("/certs.ar"));
  int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.print(F("Number of CA certs read: "));
  Serial.println(numCerts);
  if (numCerts == 0) {
    Serial.println(F("No certs found. Did you run certs-from-mozill.py and upload the LittleFS directory before running?"));
    return; // Can't connect to anything w/o certs!
  }
  client.setCertStore(&certStore);
  mqtt_setup();
  client_tls_setup();
}



void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    if (!pubsub_client.connected()) {
        reconnect();
    }
    pubsub_client.loop();

    }
}
