#include "PushService.h"
#include <cassert>
#include "upnp++/UpnpContextManager.h"

PushService::~PushService() 
{
    
}

PushService::PushService() 
{
    
}

PushService & PushService::Instance() 
{
    static PushService service;
    return service;
}

bool PushService::InitUPnp(int port) 
{
    auto pCM = UpnpContextManager::CreateFull(
        UpnpContextManager::SSDPVersion::GSSDP_UDA_VERSION_1_0, 
        UpnpContextManager::SOCKET_FAMILY::F_IPV4,
        port
    );

    assert(pCM);

    pCM->OnContextAvailable(
        std::bind(&PushService::OnContextAvailable, this, std::placeholders::_1, std::placeholders::_2)
    );

    return true;
}

void PushService::OnContextAvailable(const UpnpContextManager &cm, const UpnpContext &c)
{
    auto cp = c.CreateMediaRendererControlPoint();
    cp->OnDeviceAvailable(std::bind(&PushService::OnDeviceOnline, this, std::placeholders::_1, std::placeholders::_2));
    cp->OnDeviceUnavailable(std::bind(&PushService::OnDeviceOffline, this, std::placeholders::_1, std::placeholders::_2));
}



void PushService::OnDeviceOnline(const UpnpControlPoint&, const UpnpDeviceProxy&) 
{
    
}

void PushService::OnDeviceOffline(const UpnpControlPoint&, const UpnpDeviceProxy&) 
{
    
}
