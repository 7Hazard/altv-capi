#!/bin/bash

rm -f -r client
mkdir -p client
cd client

../../tools/altv-api-tool \
../../cpp-sdk/SDK.h \
 \
-- \
 \
-DUNICODE \
-D_UNICODE \
-DALT_CLIENT \
-DALT_CLIENT_API \
 \
-fms-compatibility \
-fms-extensions \
-fdelayed-template-parsing \
 \
-Wno-deprecated-declarations \
-Wno-switch-enum \
-Wno-unused-command-line-argument \
-Wno-macro-redefined \
-Wno-inconsistent-missing-override \
-Wno-deprecated-declarations \
-Wno-return-type \
-Wno-switch-enum \
-Wno-switch \
-Wno-parentheses \
-Wno-unused-private-field \
-Wno-unused-variable \
-Wno-format \
-Wno-reorder \
-Wno-delete-non-virtual-dtor \
-Wno-microsoft-cast \
-Wno-unused-function \
-Wno-unused-lambda-capture \
-Wno-missing-braces \
-Wno-unused-local-typedef \
-Wno-tautological-constant-out-of-range-compare \
-Wno-delete-incomplete \
-Wno-c++11-narrowing \
-Wno-comment \
 \
-xc++ \
-ferror-limit=0 \
-Wno-pragma-once-outside-header \
-std=c++17 \
-Wno-switch-enum \
-Wno-switch \
 \
-fparse-all-comments \
 \
-nostdinc \
-fms-compatibility-version=19.11 \
/premissive- \
--target=x86_64-windows-msvc \
-isystem/WINDOWS/SDK/Include/10.0.17763.0/ucrt \
-isystem/WINDOWS/SDK/Include/10.0.17763.0/shared \
-isystem/WINDOWS/SDK/Include/10.0.17763.0/um \
-isystem/WINDOWS/SDK/Include/10.0.17763.0/winrt \
-isystem/WINDOWS/VC/Tools/MSVC/14.16.27023/include
