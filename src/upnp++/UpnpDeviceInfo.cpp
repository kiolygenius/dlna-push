#include "UpnpDeviceInfo.h"


UpnpDeviceInfo::UpnpDeviceInfo(UpnpDeviceInfo::RawHandlerPtr p)
: handler(p)
{
    
}

UpnpDeviceInfo::~UpnpDeviceInfo() 
{
    // shall not unref handler, for it is managered by context manager
}

UpnpAVTransportServiceProxy::SPtr UpnpDeviceInfo::GetAVTransport() const 
{
    if (!av_transport)    
    {
        auto p = UpnpAVTransportServiceProxy::Create(
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
        return av_transport;
    }
}

std::string UpnpDeviceInfo::Name() const 
{
    auto raw_c_string = gupnp_device_info_get_friendly_name(handler);
    std::string result(raw_c_string);
    g_free(raw_c_string);
    return result;
}

std::string UpnpDeviceInfo::Udn() const 
{
    return gupnp_device_info_get_udn(handler);
}
