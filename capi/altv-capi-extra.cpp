
/**
 * CAPI Extra features
 * This file is managed manually
 */

#include "altv-capi-sourcedefs.hpp"

#include <cassert>
#define FnAssert(fn) assert(fn && #fn " has to be a valid callback function")

CAPI unsigned int alt_GetSDKVersion()
{
    return ICore::SDK_VERSION;
}

class CAPIScriptRuntime : public alt::IScriptRuntime
{
public:

    class Resource_Impl : public alt::IResource::Impl
    {
    public:
        // Callbacks
#ifdef ALT_SERVER_API
        _Bool(*MakeClientFn)(alt_IResource*, alt_IResource_CreationInfo*, alt_Array_String*);
#endif
        _Bool(*StartFn)(alt_IResource*);
        _Bool(*StopFn)(alt_IResource*);
        _Bool(*OnEventFn)(alt_IResource*, alt_CEvent*);
        void(*OnTickFn)(alt_IResource*);
        void(*OnCreateBaseObjectFn)(alt_IResource*, alt_IBaseObject*);
        void(*OnRemoveBaseObjectFn)(alt_IResource*, alt_IBaseObject*);

        // Data
        alt_IResource* resource;
        void* extra;

        Resource_Impl(
            alt_IResource* resource,
    #ifdef ALT_SERVER_API
            _Bool(*MakeClientFn)(alt_IResource*, alt_IResource_CreationInfo*, alt_Array_String*),
    #endif
            _Bool(*StartFn)(alt_IResource*),
            _Bool(*StopFn)(alt_IResource*),
            _Bool(*OnEventFn)(alt_IResource*, alt_CEvent*),
            void(*OnTickFn)(alt_IResource*),
            void(*OnCreateBaseObjectFn)(alt_IResource*, alt_IBaseObject*),
            void(*OnRemoveBaseObjectFn)(alt_IResource*, alt_IBaseObject*)
        ) : alt::IResource::Impl(),
#ifdef ALT_SERVER_API
            MakeClientFn(MakeClientFn),
#endif
            StartFn(StartFn),
            StopFn(StopFn),
            OnEventFn(OnEventFn),
            OnTickFn(OnTickFn),
            OnCreateBaseObjectFn(OnCreateBaseObjectFn),
            OnRemoveBaseObjectFn(OnRemoveBaseObjectFn)
        {}

#ifdef ALT_SERVER_API
		virtual bool MakeClient(alt::IResource::CreationInfo* info, alt::Array<alt::String> files) 
        {
            return MakeClientFn((alt_IResource*)resource, (alt_IResource_CreationInfo*)info, (alt_Array_String*)&files);
        }
#endif

		virtual bool Start() {
            return StartFn((alt_IResource*)resource);
        };

		virtual bool Stop() {
            return StopFn((alt_IResource*)resource); 
        };

		virtual bool OnEvent(const alt::CEvent* ev) {
            return OnEventFn((alt_IResource*)resource, (alt_CEvent*)ev);
        };

		virtual void OnTick() {
            OnTickFn((alt_IResource*)resource);
        };

		virtual void OnCreateBaseObject(alt::IBaseObject* object) {
            OnCreateBaseObjectFn((alt_IResource*)resource, (alt_IBaseObject*)object);
        };

		virtual void OnRemoveBaseObject(alt::IBaseObject* object) {
            OnRemoveBaseObjectFn((alt_IResource*)resource, (alt_IBaseObject*)object);
        };
    };


    alt_IResource_Impl*(*CreateImplFn)(alt_IScriptRuntime*, alt_IResource*);
    void(*DestroyImplFn)(alt_IScriptRuntime*, alt_IResource_Impl*);
    void(*OnTickFn)(alt_IScriptRuntime*);

    CAPIScriptRuntime(
        alt_IResource_Impl*(*CreateImplFn)(alt_IScriptRuntime*, alt_IResource*),
        void(*DestroyImplFn)(alt_IScriptRuntime*, alt_IResource_Impl*),
        void(*OnTickFn)(alt_IScriptRuntime*)
    ) : CreateImplFn(CreateImplFn),
        DestroyImplFn(DestroyImplFn),
        OnTickFn(OnTickFn)
    {}
    
    virtual alt::IResource::Impl* CreateImpl(alt::IResource* resource) override
    {
        return (alt::IResource::Impl*)CreateImplFn((alt_IScriptRuntime*)this, (alt_IResource*)resource);
    }

    virtual void DestroyImpl(alt::IResource::Impl* impl) override
    {
        DestroyImplFn((alt_IScriptRuntime*)this, (alt_IResource_Impl*)impl);
    }

    virtual void OnTick() override
    {
        OnTickFn((alt_IScriptRuntime*)this);
    }
};

CAPI alt_IScriptRuntime* alt_CAPIScriptRuntime_Create(
    alt_IResource_Impl*(*CreateImplFn)(alt_IScriptRuntime*, alt_IResource*),
    void(*DestroyImplFn)(alt_IScriptRuntime*, alt_IResource_Impl*),
    void(*OnTickFn)(alt_IScriptRuntime*)
)
{
    FnAssert(CreateImplFn);
    FnAssert(DestroyImplFn);
    FnAssert(OnTickFn);

    return (alt_IScriptRuntime*)static_cast<alt::IScriptRuntime*>(
        new CAPIScriptRuntime(
            CreateImplFn,
            DestroyImplFn,
            OnTickFn
        )
    );
}

CAPI alt_IResource_Impl* alt_CAPIResource_Impl_Create(
    alt_IResource* resource,
#ifdef ALT_SERVER_API
    _Bool(*MakeClientFn)(alt_IResource*, alt_IResource_CreationInfo*, alt_Array_String*),
#endif
    _Bool(*StartFn)(alt_IResource*),
    _Bool(*StopFn)(alt_IResource*),
    _Bool(*OnEventFn)(alt_IResource*, alt_CEvent*),
    void(*OnTickFn)(alt_IResource*),
    void(*OnCreateBaseObjectFn)(alt_IResource*, alt_IBaseObject*),
    void(*OnRemoveBaseObjectFn)(alt_IResource*, alt_IBaseObject*)
)
{
#ifdef ALT_SERVER_API
    FnAssert(MakeClientFn);
#endif
    FnAssert(StartFn);
    FnAssert(StopFn);
    FnAssert(OnEventFn);
    FnAssert(OnTickFn);
    FnAssert(OnCreateBaseObjectFn);
    FnAssert(OnRemoveBaseObjectFn);

    return (alt_IResource_Impl*)static_cast<alt::IResource::Impl*>(
        new CAPIScriptRuntime::Resource_Impl(
            resource,
#ifdef ALT_SERVER_API
            MakeClientFn,
#endif
            StartFn,
            StopFn,
            OnEventFn,
            OnTickFn,
            OnCreateBaseObjectFn,
            OnRemoveBaseObjectFn
        )
    );
}

CAPI void alt_CAPIResource_Impl_SetExtra(alt_IResource_Impl* resource, void* extra)
{
    static_cast<CAPIScriptRuntime::Resource_Impl*>((alt::IResource::Impl*)resource)->extra = extra;
}

CAPI void* alt_CAPIResource_Impl_GetExtra(alt_IResource_Impl* resource)
{
    return static_cast<CAPIScriptRuntime::Resource_Impl*>((alt::IResource::Impl*)resource)->extra;
}
