#!/bin/bash
cd ../../ide/ex_project && pio run
python -m http.server --directory ./.pio/build/nodemcuv2/ --bind 192.168.178.123 8080
