#pragma once

#include "cpp-sdk/SDK.h"

#ifdef CAPI_DLL
#define CAPI CAPI_EXPORT
#else
#define CAPI CAPI_EXTERN
#endif // CAPI_DLL

#ifdef ALT_SERVER_API
#include "../alt-config/alt-config.h"
#endif
