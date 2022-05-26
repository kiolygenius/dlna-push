#ifndef _UPNP_AV_TRANSPORT_SERVICE_H_
#define _UPNP_AV_TRANSPORT_SERVICE_H_

#include <string>
#include <functional>
#include <optional>
#include <map>
#include <memory>
#include "UpnpServiceProxy.h"

class UpnpAVTransportServiceProxy : public UpnpServiceProxy
{
public:
    typedef std::shared_ptr<UpnpAVTransportServiceProxy> SPtr;
    typedef std::weak_ptr<UpnpAVTransportServiceProxy> WPtr;
    typedef std::function<void(bool)> BoolCallbackT;

    UpnpAVTransportServiceProxy(RawhandlerPtr p);
    ~UpnpAVTransportServiceProxy() override;
    struct ControlType
    {
        static const char *const Play;
        static const char *const Pause;
        static const char *const Stop;
        const char * type;
        ControlType(const char* const t) : type(t) {}
    };

    void SetUri(const std::string &, const std::optional<BoolCallbackT> &);
    void Control(ControlType type, const std::optional<BoolCallbackT> &);

private:
    static void set_av_transport_uri_cb(GObject *object, GAsyncResult *res, gpointer user_data);
    static void av_transport_action_cb (GObject *object, GAsyncResult *res, gpointer user_data);
    static std::map<RawhandlerPtr, WPtr> s_raw_callback_mapper;
    std::map<uintptr_t, std::any>  callback_mapper;
    uintptr_t last_callback_key;
};

#endif //_UPNP_AV_TRANSPORT_SERVICE_H_