#pragma once

#include "cpp-sdk/SDK.h"

#ifdef CAPI_DLL
#define CAPI CAPI_EXPORT
#else
#define CAPI CAPI_EXTERN
#endif // CAPI_DLL

#include "altv-capi.hpp"
