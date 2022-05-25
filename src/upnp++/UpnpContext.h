#ifndef _UPNP_CONTEXT_H_
#define _UPNP_CONTEXT_H_

#include <memory>
#include <libgupnp/gupnp-context.h>
#include "UpnpControlPoint.h"

class UpnpContext
{
public:
    typedef std::shared_ptr<UpnpContext> SPtr;
    typedef std::weak_ptr<UpnpContext> WPtr;
    typedef GUPnPContext *RawHandlerPtr;
    UpnpContext(RawHandlerPtr);
    ~UpnpContext();

    UpnpControlPoint::SPtr CreateMediaRendererControlPoint() const;
private:
    GUPnPContext* context;
};

#endif// _UPNP_CONTEXT_H_