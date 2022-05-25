#ifndef _PUSH_SERVICE_H_
#define _PUSH_SERVICE_H_

#include "upnp++/UpnpContextManager.h"
#include "upnp++/UpnpContext.h"

class PushService
{
public:
    static PushService &Instance();
    ~PushService();
    bool InitUPnp(int port);

private:
    PushService();
    void OnContextAvailable(const UpnpContextManager&, const UpnpContext&);
    void OnDeviceOnline(const UpnpControlPoint&, const UpnpDeviceProxy&);
    void OnDeviceOffline(const UpnpControlPoint&, const UpnpDeviceProxy&);
};


#endif // _PUSH_SERVICE_H_