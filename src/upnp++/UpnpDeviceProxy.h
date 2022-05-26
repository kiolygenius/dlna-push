#ifndef _UPNP_DEVICE_PROXY_H_
#define _UPNP_DEVICE_PROXY_H_

#include <libgupnp/gupnp-device-proxy.h>
#include <memory>
#include <functional>
#include "UpnpDeviceInfo.h"

class UpnpDeviceProxy
{
public:
    typedef GUPnPDeviceProxy *RawHandlerPtr;
    typedef std::shared_ptr<UpnpDeviceProxy> SPtr;
    typedef std::weak_ptr<UpnpDeviceProxy> WPtr;
    typedef std::function<bool(const UpnpDeviceProxy::SPtr&, const UpnpDeviceProxy::SPtr&)> Comparator;
    typedef std::function<bool(const UpnpDeviceProxy::SPtr&, RawHandlerPtr)> RawEqualComparator;
    
    UpnpDeviceProxy(RawHandlerPtr);
    ~UpnpDeviceProxy();

    UpnpDeviceInfo::SPtr GetDeviceInfo() const;

    static RawEqualComparator DefaultRawEqualComparator();
    static Comparator DefaultComparator();

private:
    mutable UpnpDeviceInfo::WPtr device_info;

    RawHandlerPtr handler;
};

#endif // _UPNP_DEVICE_PROXY_H_
