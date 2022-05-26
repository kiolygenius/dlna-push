#include "UpnpDeviceProxy.h"


UpnpDeviceProxy::UpnpDeviceProxy(RawHandlerPtr p) 
: handler(p)
{
    
}

UpnpDeviceProxy::~UpnpDeviceProxy() 
{
    g_object_unref(handler);
}


UpnpDeviceProxy::Comparator UpnpDeviceProxy::DefaultComparator() 
{
    return [](const UpnpDeviceProxy::SPtr &left, const UpnpDeviceProxy::SPtr &right)->bool {
        return left->handler < right->handler;
    };
}


UpnpDeviceProxy::RawEqualComparator UpnpDeviceProxy::DefaultRawEqualComparator() 
{
    return [](const UpnpDeviceProxy::SPtr &o, RawHandlerPtr p)->bool {
        return p == o->handler;
    };
}


UpnpDeviceInfo::SPtr UpnpDeviceProxy::GetDeviceInfo() const
{
    if (device_info.expired()) {
        auto p = std::make_shared<UpnpDeviceInfo>(GUPNP_DEVICE_INFO(
            handler
        ));
        device_info = p;
        return p;
    }
    else
    {
        return device_info.lock();
    }
}
