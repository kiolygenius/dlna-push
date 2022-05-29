#ifndef _UPNP_DEVICE_INFO_H_
#define _UPNP_DEVICE_INFO_H_

#include <memory>
#include <libgupnp/gupnp-device-info.h>
#include "UpnpAVTransportServiceProxy.h"

class UpnpDeviceInfo
{
public:
    typedef std::shared_ptr<UpnpDeviceInfo> SPtr;
    typedef std::weak_ptr<UpnpDeviceInfo> WPtr;
    typedef GUPnPDeviceInfo *RawHandlerPtr;

    const char * const AV_TRANSPORT = "urn:schemas-upnp-org:service:AVTransport";

    UpnpDeviceInfo(RawHandlerPtr);
    ~UpnpDeviceInfo();

    std::string Name() const;
    std::string Udn() const;

    UpnpAVTransportServiceProxy::SPtr GetAVTransport() const;

private:
    mutable UpnpAVTransportServiceProxy::SPtr av_transport;

    RawHandlerPtr handler;
};

#endif