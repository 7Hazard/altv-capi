#pragma once

#include "cpp-sdk/SDK.h"
#include "altv-capi.hpp"
#undef CAPI
#ifdef CAPI_DLL
#define CAPI CAPI_EXPORT
#else
#define CAPI CAPI_EXTERN
#endif // CAPI_DLL
