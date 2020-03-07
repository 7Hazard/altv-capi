#pragma once

#include "cpp-sdk/SDK.h"

#ifdef CAPI_DLL
#define CAPI CAPI_EXPORT
#else
#define CAPI CAPI_EXTERN
#endif // CAPI_DLL

using nullptr_t = std::nullptr_t;

#include "altv-capi.hpp"
