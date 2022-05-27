#include "PushService.h"
#include <cassert>
#include "upnp++/UpnpContextManager.h"
#include <iostream>
#include <chrono>

PushService::PushService() 
: devices(UpnpDeviceProxy::DefaultComparator())
, finish(false)
, task_pending {}
{
    
}

PushService::~PushService() 
{
    
}

void PushService::PoolOver()
{
    if (!finish && task_pending)
    {
        std::any param(nullptr);
        std::optional<std::any> ret;
        int n = 0;
        do {
            ret = task_pending.value()(param);
            if (ret)
            {
                param = ret.value();
            }
        } while (ret);
    }
}

bool PushService::IsFinished() const
{
    return finish;
}

bool PushService::InitUPnp(int port) 
{
    pCM = UpnpContextManager::CreateFull(
        UpnpContextManager::SSDPVersion::GSSDP_UDA_VERSION_1_1, 
        UpnpContextManager::SOCKET_FAMILY::F_IPV4,
        port
    );

    assert(pCM);

    pCM->OnContextAvailable(
        std::bind(&PushService::OnContextAvailable, this, std::placeholders::_1, std::placeholders::_2)
    );

    return true;
}

void PushService::OnContextAvailable(const UpnpContextManager::SPtr &cm, const UpnpContext::SPtr &c)
{
    auto pCP = c->CreateMediaRendererControlPoint();
    pCP->OnDeviceAvailable(std::bind(&PushService::OnDeviceOnline, this, std::placeholders::_1, std::placeholders::_2));
    pCP->OnDeviceUnavailable(std::bind(&PushService::OnDeviceOffline, this, std::placeholders::_1, std::placeholders::_2));
    pCP->ActiveResourceBrowser(true);
    pCM->TakeOverControlPoint(pCP);
}



void PushService::OnDeviceOnline(const UpnpControlPoint::SPtr &cp, const UpnpDeviceProxy::SPtr &d) 
{
    if (devices.cend() == devices.find(d))
    {
        devices.insert(d);
        // std::cerr << d->GetDeviceInfo()->Name() << '\n';
    }
}

void PushService::OnDeviceOffline(const UpnpControlPoint::SPtr &cp, const UpnpDeviceProxy::SPtr &d) 
{
    auto it = devices.find(d);
    if (it != devices.cend())
    {
        devices.erase(it);
    }
}

void PushService::Play(const std::string &device, const std::string &uri)
{    
    TaskT push_url = [device, uri, this](std::any p)->std::optional<std::any> 
    {
        auto it = std::any_cast<decltype(devices)::const_iterator>(p);
        auto pService = (*it)->GetDeviceInfo()->GetAVTransport();
        pService->SetUri(uri, [pService, device, uri, this](bool result) 
        {
            if (!result) 
            {
                std::cerr << "SetUri for media renderer failed.\n"
                    << "[Device]: " << device << '\n'
                    << "[URL]: " << uri << '\n';
                finish = true;
            }
            else
            {
                pService->Control(UpnpAVTransportServiceProxy::ControlType::Play, [pService, device, uri, this](bool result)
                {
                    if (!result)
                    {
                        std::cerr << "Play fail.\n";
                        std::cerr << "[Device]: " << device << '\n';
                        std::cerr << "[URL]: " << uri << '\n';
                    }
                    finish = true;
                    task_pending = {};
                });
            }
        });
        task_pending = {};
        return {};
    };

    TaskT find_device = [push_url, device, this](std::any)->std::optional<std::any> 
    {
        decltype(devices)::const_iterator it = std::find_if(devices.cbegin(), devices.cend(), [&device](const UpnpDeviceProxy::SPtr &p) { return p->GetDeviceInfo()->Udn().find(device) != std::string::npos; });
        if (it != devices.cend()) 
        {
            task_pending = push_url;
            // Found
            return std::optional<std::any>(it);
        }
        else
        {
            return {};
        }
    };

    task_pending = find_device;
}

void PushService::List(int interval)
{
    TaskT list_device = [this](std::any)->std::optional<std::any> 
    {
        std::for_each(devices.cbegin(), devices.cend(), [](const UpnpDeviceProxy::SPtr &p) 
        {
            std::cout << "> " << p->GetDeviceInfo()->Name() 
                << " (" << p->GetDeviceInfo()->Udn() << ")"
                << '\n';
        });
        std::cout << "\n" << devices.size() << " devices found." << '\n';
        finish = true;
        task_pending = {};
        return {};
    };

    auto start = std::chrono::steady_clock::now();
    TaskT timewait = [start, interval, list_device, this](std::any)->std::optional<std::any> 
    {
        std::chrono::duration<double> passed = std::chrono::steady_clock::now() - start;
        if (passed.count() > interval)
        {
            task_pending = list_device;
            return std::any(true);
        }
        else
        {
            return {};
        }
    };

    task_pending = timewait;
}
