
/**
 * CAPI Extra features
 * This file is managed manually
 */

#ifndef _CAPI_EXTRA_H_
#define _CAPI_EXTRA_H_

CAPI unsigned int alt_GetSDKVersion();
#define CAPI_EXPORT_SDK_VERSION CAPI_EXPORT unsigned int GetSDKVersion() { return alt_GetSDKVersion(); }

CAPI alt_IScriptRuntime* alt_IScriptRuntime_Create(
    alt_IResource*(*CreateResourceFn)(alt_IResource_CreationInfo*),
    void(*RemoveResourceFn)(alt_IResource*),
    void(*OnTickFn)()
);

CAPI alt_IResource* alt_IResource_Create(
    alt_IResource_CreationInfo* info,
#ifdef ALT_SERVER_API
    void(*MakeClientFn)(alt_IResource_CreationInfo*, alt_ArrayString*),
#endif
    _Bool(*InstantiateFn)(),
    _Bool(*StartFn)(),
    _Bool(*StopFn)(),
    _Bool(*OnEventFn)(alt_CEvent*),
    void(*OnTickFn)(),
    void(*OnCreateBaseObjectFn)(alt_IBaseObject*),
    void(*OnRemoveBaseObjectFn)(alt_IBaseObject*)
);

#endif // _CAPI_EXTRA_H_
