#include "UpnpDeviceInfo.h"


UpnpDeviceInfo::UpnpDeviceInfo(UpnpDeviceInfo::RawHandlerPtr p)
: handler(p)
{
    
}

UpnpDeviceInfo::~UpnpDeviceInfo() 
{
    g_object_unref(handler);
}

UpnpAVTransportServiceProxy::SPtr UpnpDeviceInfo::GetAVTransport() const 
{
    if (av_transport.expired())    
    {
        auto p = std::make_shared<UpnpAVTransportServiceProxy>(
            GUPNP_SERVICE_PROXY(
                gupnp_device_info_get_service(
                    handler,
                    AV_TRANSPORT
                )
            )
        );
        av_transport = p;
        return p;
    }
    else
    {
        return av_transport.lock();
    }
}

std::string UpnpDeviceInfo::Name() const 
{
    auto raw_c_string = gupnp_device_info_get_friendly_name(handler);
    std::string result(raw_c_string);
    g_free(raw_c_string);
    return result;
}
