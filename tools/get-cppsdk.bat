@echo off
:: adapted for windows 10
:: CALL FROM PROJ ROOT DIR | ./tools/get-cppsdk.bat

if exist cpp-sdk rd /s /q cpp-sdk
echo Fetching latest alt:V CPP-SDK
powershell -Command "Invoke-WebRequest https://github.com/altmp/cpp-sdk/archive/master.zip -OutFile cppsdk.zip"
powershell -Command "Expand-Archive -Force cppsdk.zip ."
del cppsdk.zip
move cpp-sdk-master cpp-sdk
echo Fetched latest alt:V CPP-SDK
