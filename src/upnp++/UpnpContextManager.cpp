#include "UpnpContextManager.h"
#include "UpnpResourceFactory.h"

std::map<UpnpContextManager::RawHandlerType, UpnpContextManager::WPtr> UpnpContextManager::s_cm_mapper;

UpnpContextManager::UpnpContextManager(GUPnPContextManager *handler)
: pcm(handler), last_mapper_key(0)
{

}

UpnpContextManager::~UpnpContextManager()
{
    g_object_unref(pcm);
}

UpnpContextManager::SPtr UpnpContextManager::CreateFull(UpnpContextManager::SSDPVersion ssdp_v, SOCKET_FAMILY family, int port)
{
    auto raw = gupnp_context_manager_create_full(ssdp_v, (GSocketFamily)family, port);
    auto p = SPtr(new UpnpContextManager(
        raw
    ));
    s_cm_mapper.insert(make_pair(raw, std::weak_ptr<UpnpContextManager>(p)));
    return p;
}

void UpnpContextManager::raw_context_available_cb(GUPnPContextManager *cm, GUPnPContext *context, gpointer user_data)
{
    auto it = s_cm_mapper.find(cm);
    if (it != s_cm_mapper.cend() && !it->second.expired())
    {
        auto p = it->second.lock();
        auto cb_it = p->callback_mapper.find(reinterpret_cast<uintptr_t>(user_data));
        if (cb_it != p->callback_mapper.cend()) 
        {
            if (context) 
            {
                auto ctx = std::make_shared<UpnpContext>(context);
                p->contexts.push_back(ctx);
                (cb_it->second)(p, ctx);
            }
        }
    }
}


unsigned long UpnpContextManager::SignalConnect(const std::string &signal, GCallback cb, uintptr_t new_key)
{
    return g_signal_connect(
        pcm,
        signal.c_str(),
        cb,
        reinterpret_cast<void*>(new_key)
    );
}

unsigned long UpnpContextManager::OnContextAvailable(const std::function<void(const UpnpContextManager::SPtr&, const UpnpContext::SPtr&)> &callback)
{
    uintptr_t new_key = ++last_mapper_key;
    callback_mapper.insert(std::make_pair(new_key, callback));
    return SignalConnect("context-available", G_CALLBACK(&UpnpContextManager::raw_context_available_cb), new_key);
}

void UpnpContextManager::TakeOverControlPoint(const UpnpControlPoint::SPtr & cp)
{
    gupnp_context_manager_manage_control_point(pcm, cp->GetRawHandler());
    control_points.push_back(cp);
}
