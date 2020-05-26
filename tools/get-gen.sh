#!/bin/bash

## CALL FROM PROJ ROOT DIR | ./tools/get-cppsdk.sh

rm -f tools/altv-capi-gen
echo "Fetching latest alt:V CAPI Generator"
curl -sSL https://github.com/7Hazard/altv-capi/releases/latest/download/altv-capi-gen -o tools/altv-capi-gen
echo "Fetched latest alt:V CAPI Generator"
