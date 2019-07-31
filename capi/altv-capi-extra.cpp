
/**
 * CAPI Extra features
 * This file is managed manually
 */

#include "altv-capi-sourcedefs.hpp"

#include <cassert>
#define FnAssert(fn) assert(fn && #fn " has to be a valid callback function")

CAPI unsigned int alt_GetSDKVersion()
{
    return ALTV_SDK_VERSION;
}

class CAPIScriptRuntime : public alt::IScriptRuntime
{
public:

    class Resource : public alt::IResource
    {
    public:
        // Callbacks
#ifdef ALT_SERVER_API
        void(*MakeClientFn)(alt_IResource*, alt_IResource_CreationInfo*, alt_Array_String*);
#endif
        _Bool(*InstantiateFn)(alt_IResource*);
        _Bool(*StartFn)(alt_IResource*);
        _Bool(*StopFn)(alt_IResource*);
        _Bool(*OnEventFn)(alt_IResource*, alt_CEvent*);
        void(*OnTickFn)(alt_IResource*);
        void(*OnCreateBaseObjectFn)(alt_IResource*, alt_IBaseObject*);
        void(*OnRemoveBaseObjectFn)(alt_IResource*, alt_IBaseObject*);

        // Extras
        void* extra;

        Resource(
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
        ) : alt::IResource((alt::IResource::CreationInfo*)info),
#ifdef ALT_SERVER_API
            MakeClientFn(MakeClientFn),
#endif
            InstantiateFn(InstantiateFn),
            StartFn(StartFn),
            StopFn(StopFn),
            OnEventFn(OnEventFn),
            OnTickFn(OnTickFn),
            OnCreateBaseObjectFn(OnCreateBaseObjectFn),
            OnRemoveBaseObjectFn(OnRemoveBaseObjectFn)
        {}

#ifdef ALT_SERVER_API
		virtual void MakeClient(CreationInfo* info, alt::Array<alt::String> files) 
        {
            MakeClientFn((alt_IResource*)this, (alt_IResource_CreationInfo*)info, (alt_Array_String*)&files);
        }
#endif
        
		virtual bool Instantiate() {
            state = State::INSTANTIATING;
            return InstantiateFn((alt_IResource*)this);
        }

		virtual bool Start() {
            state = State::STARTED;
            return StartFn((alt_IResource*)this);
        };

		virtual bool Stop() {
            state = State::STOPPED;
            return StopFn((alt_IResource*)this); 
        };

		virtual bool OnEvent(const alt::CEvent* ev) {
            return OnEventFn((alt_IResource*)this, (alt_CEvent*)ev);
        };

		virtual void OnTick() {
            OnTickFn((alt_IResource*)this);
        };

		virtual void OnCreateBaseObject(alt::IBaseObject* object) {
            OnCreateBaseObjectFn((alt_IResource*)this, (alt_IBaseObject*)object);
        };

		virtual void OnRemoveBaseObject(alt::IBaseObject* object) {
            OnRemoveBaseObjectFn((alt_IResource*)this, (alt_IBaseObject*)object);
        };
    };


    alt_IResource*(*CreateResourceFn)(alt_IScriptRuntime*, alt_IResource_CreationInfo*);
    void(*RemoveResourceFn)(alt_IScriptRuntime*, alt_IResource*);
    void(*OnTickFn)(alt_IScriptRuntime*);

    CAPIScriptRuntime(
        alt_IResource*(*CreateResourceFn)(alt_IScriptRuntime*, alt_IResource_CreationInfo*),
        void(*RemoveResourceFn)(alt_IScriptRuntime*, alt_IResource*),
        void(*OnTickFn)(alt_IScriptRuntime*)
    ) : CreateResourceFn(CreateResourceFn),
        RemoveResourceFn(RemoveResourceFn),
        OnTickFn(OnTickFn)
    {}
    
    virtual alt::IResource* CreateResource(alt::IResource::CreationInfo* info) override
    {
        return (alt::IResource*)CreateResourceFn((alt_IScriptRuntime*)this, (alt_IResource_CreationInfo*)info);
    }

    virtual void RemoveResource(alt::IResource* resource) override
    {
        RemoveResourceFn((alt_IScriptRuntime*)this, (alt_IResource*)resource);
    }

    virtual void OnTick() override
    {
        OnTickFn((alt_IScriptRuntime*)this);
    }
};

CAPI alt_IScriptRuntime* alt_CAPIScriptRuntime_Create(
    alt_IResource*(*CreateResourceFn)(alt_IScriptRuntime*, alt_IResource_CreationInfo*),
    void(*RemoveResourceFn)(alt_IScriptRuntime*, alt_IResource*),
    void(*OnTickFn)(alt_IScriptRuntime*)
)
{
    FnAssert(CreateResourceFn);
    FnAssert(RemoveResourceFn);
    FnAssert(OnTickFn);

    return (alt_IScriptRuntime*)static_cast<alt::IScriptRuntime*>(
        new CAPIScriptRuntime(
            CreateResourceFn,
            RemoveResourceFn,
            OnTickFn
        )
    );
}

CAPI alt_IResource* alt_CAPIResource_Create(
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
)
{
#ifdef ALT_SERVER_API
    FnAssert(MakeClientFn);
#endif
    FnAssert(InstantiateFn);
    FnAssert(StartFn);
    FnAssert(StopFn);
    FnAssert(OnEventFn);
    FnAssert(OnTickFn);
    FnAssert(OnCreateBaseObjectFn);
    FnAssert(OnRemoveBaseObjectFn);

    return (alt_IResource*)static_cast<alt::IResource*>(
        new CAPIScriptRuntime::Resource(
            info,
#ifdef ALT_SERVER_API
            MakeClientFn,
#endif
            InstantiateFn,
            StartFn,
            StopFn,
            OnEventFn,
            OnTickFn,
            OnCreateBaseObjectFn,
            OnRemoveBaseObjectFn
        )
    );
}

CAPI void alt_CAPIResource_SetExtra(alt::IResource* resource, void* extra)
{
    static_cast<CAPIScriptRuntime::Resource*>(resource)->extra = extra;
}

CAPI void* alt_CAPIResource_GetExtra(alt::IResource* resource)
{
    return static_cast<CAPIScriptRuntime::Resource*>(resource)->extra;
}
