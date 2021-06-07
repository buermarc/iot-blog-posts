all:
	pio run
	python tools/signing.py --mode sign \
	    --publickey keys/public.key \
	    --privatekey keys/private.key \
	    --bin ./.pio/build/nodemcuv2/firmware.bin \
	    --out ./.pio/build/nodemcuv2/firmware.bin.signed
