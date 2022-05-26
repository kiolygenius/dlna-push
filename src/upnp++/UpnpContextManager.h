#ifndef _UPNP_CONTEXT_MANAGER_H_
#define _UPNP_CONTEXT_MANAGER_H_

#include <memory>
#include <optional>
#include <functional>
#include <string>
#include <map>
#include <libgupnp/gupnp-context-manager.h>
#include "UpnpContext.h"

class UpnpContext;

class UpnpContextManager : std::enable_shared_from_this<UpnpContextManager>
{
public:
    ~UpnpContextManager();
    typedef std::shared_ptr<UpnpContextManager> SPtr;
    typedef std::weak_ptr<UpnpContextManager> WPtr;
    typedef GSSDPUDAVersion SSDPVersion;
    typedef GUPnPContextManager *RawHandlerType;
    enum SOCKET_FAMILY {
        F_IPV4 = G_SOCKET_FAMILY_IPV4,
        F_IPV6 = G_SOCKET_FAMILY_IPV6
    };
    static SPtr CreateFull(SSDPVersion, SOCKET_FAMILY, int);
    unsigned long OnContextAvailable(const std::function<void(const UpnpContextManager::SPtr&, const UpnpContext::SPtr&)> &);
    void TakeOverControlPoint(const UpnpControlPoint::SPtr &);

private:
    UpnpContextManager(GUPnPContextManager*);
    unsigned long SignalConnect(const std::string &signal, GCallback cb, uintptr_t new_key);
    static void raw_context_available_cb(GUPnPContextManager *cm, GUPnPContext *context, gpointer user_data);
private:
    RawHandlerType pcm;
    std::vector<UpnpContext::SPtr> contexts;
    std::vector<UpnpControlPoint::SPtr> control_points;
    std::map<int, std::function<void(const UpnpContextManager::SPtr&, const UpnpContext::SPtr&)> > callback_mapper;
    uintptr_t last_mapper_key;

    static std::map<RawHandlerType, WPtr> s_cm_mapper;
};

#endif // _UPNP_CONTEXT_MANAGER_