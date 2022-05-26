#include "UpnpContext.h"


UpnpContext::UpnpContext(UpnpContext::RawHandlerPtr raw) 
: context(raw)
{
    
}

UpnpContext::~UpnpContext() 
{
    if (context)
    {
        g_object_unref(context);
        context = nullptr;
    }
}

UpnpControlPoint::SPtr UpnpContext::CreateMediaRendererControlPoint() const
{
    constexpr auto RENDERER = "urn:schemas-upnp-org:device:MediaRenderer:1";
    return std::make_shared<UpnpControlPoint>(
        gupnp_control_point_new(context, RENDERER)
    );
}
