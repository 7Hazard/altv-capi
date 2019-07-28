
/**
 * CAPI Extra features
 * This file is managed manually
 */

#ifndef _CAPI_EXTRA_H_
#define _CAPI_EXTRA_H_

CAPI unsigned int alt_GetSDKVersion();
#define CAPI_EXPORT_SDK_VERSION CAPI_EXPORT unsigned int GetSDKVersion() { return alt_GetSDKVersion(); }

CAPI alt_IScriptRuntime* alt_CAPI_IScriptRuntime_Create(
    alt_IResource*(*CreateResourceFn)(alt_IScriptRuntime*, alt_IResource_CreationInfo*),
    void(*RemoveResourceFn)(alt_IScriptRuntime*, alt_IResource*),
    void(*OnTickFn)(alt_IScriptRuntime*)
);

CAPI alt_IResource* alt_CAPI_IResource_Create(
    alt_IResource_CreationInfo* info,
#ifdef ALT_SERVER_API
    void(*MakeClientFn)(alt_IResource*, alt_IResource_CreationInfo*, alt_Array_String*),
#endif
    _Bool(*InstantiateFn)(alt_IResource*),
    _Bool(*StartFn)(alt_IResource*),
    _Bool(*StopFn)(alt_IResource*),
    _Bool(*OnEventFn)(alt_IResource*, alt_CEvent*),
    void(*OnTickFn)(alt_IResource*),
    void(*OnCreateBaseObjectFn)(alt_IResource*, alt_IBaseObject*),
    void(*OnRemoveBaseObjectFn)(alt_IResource*, alt_IBaseObject*)
);

#endif // _CAPI_EXTRA_H_
