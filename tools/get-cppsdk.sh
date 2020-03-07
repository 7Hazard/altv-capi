#!/bin/bash

## CALL FROM PROJ ROOT DIR | ./tools/get-cppsdk.sh

rm -rf cpp-sdk
echo "Fetching latest alt:V CPP-SDK"
curl -sSL https://github.com/altmp/cpp-sdk/archive/master.zip -o cppsdk.zip
unzip -qo cppsdk.zip
rm -rf cppsdk.zip
mv cpp-sdk-master cpp-sdk
echo "Fetched latest alt:V CPP-SDK"
