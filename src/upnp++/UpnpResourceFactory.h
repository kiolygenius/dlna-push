#ifndef _UPNP_RESOURCE_FACTORY_H_
#define _UPNP_RESOURCE_FACTORY_H_

#include <libgupnp/gupnp-control-point.h>
#include <libgupnp-av/gupnp-av.h>
#include <string>

class UpnpResourceFactory
{
public:
    enum ControlPointType {
        MEDIA_SERVER,
        MEDIA_RENDERER
    };

    UpnpResourceFactory();
    ~UpnpResourceFactory();

    GUPnPResourceFactory *getRawHandler() const;
private:
    GUPnPResourceFactory *factory;
};

#endif  //_UPNP_RESOURCE_FACTORY_