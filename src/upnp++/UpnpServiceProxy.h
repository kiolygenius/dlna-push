#ifndef _UPNP_SERVICE_PROXY_H_
#define _UPNP_SERVICE_PROXY_H_

#include <memory>
#include <map>
#include <any>
#include <functional>
#include <libgupnp/gupnp-service-proxy.h>

class UpnpServiceProxy : public std::enable_shared_from_this<UpnpServiceProxy>
{
public:
    typedef std::shared_ptr<UpnpServiceProxy> SPtr;
    typedef std::weak_ptr<UpnpServiceProxy> WPtr;
    typedef GUPnPServiceProxy *RawhandlerPtr;

    UpnpServiceProxy(RawhandlerPtr);
    virtual ~UpnpServiceProxy();

protected:
    RawhandlerPtr handler;
};

#endif //_UPNP_SERVICE_PROXY_H_