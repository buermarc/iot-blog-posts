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

#include <time.h>

#include <FS.h>
#include <LittleFS.h>

#include <CertStoreBearSSL.h>
BearSSL::CertStore certStore;

#include <base64.h>

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

ESP8266WiFiMulti WiFiMulti;

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


void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    setClock();

    BearSSL::WiFiClientSecure client;
    bool mfln = client.probeMaxFragmentLength("server", 443, 1024);  // server must be the same as in ESPhttpUpdate.update()
    Serial.printf("MFLN supported: %s\n", mfln ? "yes" : "no");
    if (mfln) {
      client.setBufferSizes(1024, 1024);
    }
    client.setCertStore(&certStore);

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
    ESPhttpUpdate.onStart(update_started);
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
}
