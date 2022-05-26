#ifndef _UPNP_CONTROL_POINT_H_
#define _UPNP_CONTROL_POINT_H_

#include <memory>
#include <functional>
#include <string>
#include <map>
#include <set>
#include <libgupnp/gupnp-control-point.h>
#include "UpnpDeviceProxy.h"

class UpnpDeviceProxy;

class UpnpControlPoint
{
public:
    typedef GUPnPControlPoint* RawHandlerPtr;
    typedef std::shared_ptr<UpnpControlPoint> SPtr;
    typedef std::weak_ptr<UpnpControlPoint> WPtr;
    typedef std::function<void(const UpnpControlPoint::SPtr&, const UpnpDeviceProxy::SPtr&)> DEVICE_CALLBACK_T;
    UpnpControlPoint(RawHandlerPtr);
    ~UpnpControlPoint();
    unsigned long OnDeviceAvailable(const DEVICE_CALLBACK_T &);
    unsigned long OnDeviceUnavailable(const DEVICE_CALLBACK_T &);
    inline RawHandlerPtr GetRawHandler() const { return handler; }
    
private:
    unsigned long SignalConnect(const std::string &signal, GCallback cb, uintptr_t user_data);
    static void raw_device_available_cb(RawHandlerPtr, UpnpDeviceProxy::RawHandlerPtr, void*);
    static void raw_device_unavailable_cb(RawHandlerPtr, UpnpDeviceProxy::RawHandlerPtr, void*);

    std::map<uintptr_t, DEVICE_CALLBACK_T> callback_mapper;
    std::set<UpnpDeviceProxy::SPtr, UpnpDeviceProxy::Comparator> devices;
    uintptr_t last_callback_key;
    RawHandlerPtr handler;

    static std::map<RawHandlerPtr, WPtr> s_controlpointer_mapper;
};

#endif// _UPNP_CONTROL_POINT_H_
