@echo off
:: adapted for windows 10
:: CALL FROM PROJ ROOT DIR | ./tools/get-cppsdk.bat

if exist tools\altv-capi-gen.exe del tools\altv-capi-gen.exe
if exist tools\altv-capi-gen.exe.tmp del tools\altv-capi-gen.exe.tmp
echo Fetching latest alt:V CAPI Generator
call powershell -Command "Invoke-WebRequest https://github.com/7Hazard/altv-capi/releases/latest/download/altv-capi-gen.exe -OutFile tools/altv-capi-gen.exe.tmp"
copy tools\altv-capi-gen.exe.tmp tools\altv-capi-gen.exe
del tools\altv-capi-gen.exe.tmp
echo Fetched latest alt:V CAPI Generator
