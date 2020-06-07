#!/bin/bash

echo "GENERATING CLIENT CAPI CODE"

rm -f -r client
mkdir -p client
cd client

../../tools/altv-capi-gen \
../../src/CAPI.h \
 \
-- \
 \
-DUNICODE \
-D_UNICODE \
-DALT_CLIENT \
-DALT_CLIENT_API \
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
-Wno-pragma-once-outside-header \
-Wno-switch-enum \
-Wno-switch \
 \
-xc++ \
-ferror-limit=0 \
-std=c++17 \
-fparse-all-comments \
 \
-isystem'/usr/include/c++/8' \
-isystem'/usr/include/x86_64-linux-gnu/c++/8' \
-isystem'/usr/include/clang/9/include' \
