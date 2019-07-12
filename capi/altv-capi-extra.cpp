
/**
 * CAPI Extra features
 * This file is managed by hand
 */

#include "altv-capi-sourcedefs.hpp"

#include <cassert>
#define FnAssert(fn) assert(fn && #fn " has to be a valid callback function")

void gg(float const(&elem)[3]) {}
void gg2(float* g) { gg((float const (&)[3])g); }

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

#ifdef ALT_SERVER_API
        void(*MakeClientFn)(alt_IResource_CreationInfo*, alt_ArrayString*);
#endif
        _Bool(*InstantiateFn)();
        _Bool(*StartFn)();
        _Bool(*StopFn)();
        _Bool(*OnEventFn)(alt_CEvent*);
        void(*OnTickFn)();
        void(*OnCreateBaseObjectFn)(alt_IBaseObject*);
        void(*OnRemoveBaseObjectFn)(alt_IBaseObject*);

        Resource(
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
		virtual void MakeClient(CreationInfo* info, Array<String> files) 
        {
            MakeClientFn((alt_IResource_CreationInfo*)info, (alt_ArrayString*)&files);
        }
#endif
        
		virtual bool Instantiate() {
            state = State::INSTANTIATING;
            return InstantiateFn();
        }

		virtual bool Start() {
            state = State::STARTED;
            return StartFn();
        };

		virtual bool Stop() {
            state = State::STOPPED;
            return StopFn; 
        };

		virtual bool OnEvent(const alt::CEvent* ev) {
            return OnEventFn((alt_CEvent*)ev);
        };

		virtual void OnTick() {
            OnTickFn();
        };

		virtual void OnCreateBaseObject(alt::IBaseObject* object) {
            OnCreateBaseObjectFn((alt_IBaseObject*)object);
        };

		virtual void OnRemoveBaseObject(alt::IBaseObject* object) {
            OnRemoveBaseObjectFn((alt_IBaseObject*)object);
        };
    };

    CAPIScriptRuntime(
        alt_IResource*(*CreateResourceFn)(alt_IResource_CreationInfo*),
        void(*RemoveResourceFn)(alt_IResource*),
        void(*OnTickFn)()
    ) : CreateResourceFn(CreateResourceFn),
        RemoveResourceFn(RemoveResourceFn),
        OnTickFn(OnTickFn)
    {}

    alt_IResource*(*CreateResourceFn)(alt_IResource_CreationInfo*);
    virtual alt::IResource* CreateResource(alt::IResource::CreationInfo* info) override
    {
        return (alt::IResource*)CreateResourceFn((alt_IResource_CreationInfo*)info);
    }

    void(*RemoveResourceFn)(alt_IResource*);
    virtual void RemoveResource(alt::IResource* resource) override
    {
        RemoveResourceFn((alt_IResource*)resource);
    }

    void(*OnTickFn)();
    virtual void OnTick() override
    {
        OnTickFn();
    }
};

CAPI alt_IScriptRuntime* alt_IScriptRuntime_Create(
    alt_IResource*(*CreateResourceFn)(alt_IResource_CreationInfo*),
    void(*RemoveResourceFn)(alt_IResource*),
    void(*OnTickFn)()
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
