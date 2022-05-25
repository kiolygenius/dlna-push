#include "UpnpDeviceProxy.h"


UpnpDeviceProxy::UpnpDeviceProxy(RawHandlerPtr p) 
: handler(p)
{
    
}

UpnpDeviceProxy::~UpnpDeviceProxy() 
{
    
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
