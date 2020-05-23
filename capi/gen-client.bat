@echo off

echo GENERATING CLIENT CAPI CODE

if exist client rd /s /q client
mkdir client
cd client

"../../tools/altv-api-tool" ^
../../src/CAPI.h.h ^
 ^
-- ^
 ^
-DUNICODE ^
-D_UNICODE ^
-DALT_CLIENT ^
-DALT_CLIENT_API ^
 ^
-fms-compatibility ^
-fms-extensions ^
-fdelayed-template-parsing ^
 ^
-Wno-deprecated-declarations ^
-Wno-switch-enum ^
-Wno-unused-command-line-argument ^
-Wno-macro-redefined ^
-Wno-inconsistent-missing-override ^
-Wno-deprecated-declarations ^
-Wno-return-type ^
-Wno-switch-enum ^
-Wno-switch ^
-Wno-parentheses ^
-Wno-unused-private-field ^
-Wno-unused-variable ^
-Wno-format ^
-Wno-reorder ^
-Wno-delete-non-virtual-dtor ^
-Wno-microsoft-cast ^
-Wno-unused-function ^
-Wno-unused-lambda-capture ^
-Wno-missing-braces ^
-Wno-unused-local-typedef ^
-Wno-tautological-constant-out-of-range-compare ^
-Wno-delete-incomplete ^
-Wno-c++11-narrowing ^
-Wno-comment ^
 ^
-xc++ ^
-ferror-limit=0 ^
-Wno-pragma-once-outside-header ^
-std=c++17 ^
-Wno-switch-enum ^
-Wno-switch ^
 ^
-fparse-all-comments
