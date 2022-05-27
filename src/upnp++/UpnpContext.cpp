#include "UpnpContext.h"


UpnpContext::UpnpContext(UpnpContext::RawHandlerPtr raw) 
: context(raw)
{
    
}

UpnpContext::~UpnpContext() 
{

}

UpnpControlPoint::SPtr UpnpContext::CreateMediaRendererControlPoint() const
{
    constexpr auto RENDERER = "urn:schemas-upnp-org:device:MediaRenderer:1";
    return UpnpControlPoint::Create(
        gupnp_control_point_new(context, RENDERER)
    );
}
