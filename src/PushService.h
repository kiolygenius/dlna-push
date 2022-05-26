#ifndef _PUSH_SERVICE_H_
#define _PUSH_SERVICE_H_

#include <optional>
#include <any>
#include "upnp++/UpnpContextManager.h"
#include "upnp++/UpnpContext.h"

class PushService
{
public:
    typedef std::function<std::optional<std::any>(std::any)> TaskT;

    PushService();
    ~PushService();
    void PoolOver();
    bool IsFinished() const;
    bool InitUPnp(int port);
    void Play(const std::string &device, const std::string &uri);
    void List(int interval);

private:
    void OnContextAvailable(const UpnpContextManager::SPtr&, const UpnpContext::SPtr&);
    void OnDeviceOnline(const UpnpControlPoint::SPtr&, const UpnpDeviceProxy::SPtr&);
    void OnDeviceOffline(const UpnpControlPoint::SPtr&, const UpnpDeviceProxy::SPtr&);

    std::set<UpnpDeviceProxy::SPtr, UpnpDeviceProxy::Comparator> devices;

    bool finish;
    std::optional<TaskT> task_pending;
};


#endif // _PUSH_SERVICE_H_