#include "UpnpAVTransportServiceProxy.h"
#include <vector>
#include <cassert>

extern "C" {

void set_av_transport_uri_cb_wrapper(GObject *object, GAsyncResult *res, gpointer user_data)
{
    UpnpAVTransportServiceProxy::set_av_transport_uri_cb(object, res, user_data);
}
void av_transport_action_cb_wrapper(GObject *object, GAsyncResult *res, gpointer user_data)
{
    UpnpAVTransportServiceProxy::av_transport_action_cb(object, res, user_data);
}

}

std::map<UpnpAVTransportServiceProxy::RawhandlerPtr, UpnpAVTransportServiceProxy::WPtr> UpnpAVTransportServiceProxy::s_raw_callback_mapper;

const char *const UpnpAVTransportServiceProxy::ControlType::Play = "Play";
const char *const UpnpAVTransportServiceProxy::ControlType::Pause = "Pause";
const char *const UpnpAVTransportServiceProxy::ControlType::Stop = "Stop";

UpnpAVTransportServiceProxy::UpnpAVTransportServiceProxy(UpnpAVTransportServiceProxy::RawhandlerPtr p)
: UpnpServiceProxy(p)
{
    auto pThis = std::static_pointer_cast<UpnpAVTransportServiceProxy>(shared_from_this());
    s_raw_callback_mapper.insert(std::make_pair(p, WPtr(pThis)));
}

UpnpAVTransportServiceProxy::~UpnpAVTransportServiceProxy()
{

}

void UpnpAVTransportServiceProxy::SetUri(const std::string &uri, const std::optional<UpnpAVTransportServiceProxy::BoolCallbackT> &result_callback)
{
    constexpr auto didl_meta_format = 
        R"(<DIDL-Lite xmlns="urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:sec="http://www.sec.co.kr/" xmlns:upnp="urn:schemas-upnp-org:metadata-1-0/upnp/">
        <item id="0" parentID="-1" restricted="false">
        <dc:title>Push Video</dc:title>
        <res protocolInfo="http-get:*:audio/mpeg:*">%s</res>
        </item></DIDL-Lite>)";

    std::vector<char> didl_meta(uri.length() + strlen(didl_meta_format), '\0');
    snprintf(didl_meta.data(), didl_meta.size() - 1, didl_meta_format, uri.c_str());

    // A new action
    auto action = gupnp_service_proxy_action_new(
        "SetAVTransportURI",
        "InstanceID",
        G_TYPE_UINT,
        0,
        "CurrentURI",
        G_TYPE_STRING,
        uri,
        "CurrentURIMetaData",
        G_TYPE_STRING,
        didl_meta.data(),
        NULL
    );
    uintptr_t callback_key = 0;
    if (result_callback) {
        callback_key = ++last_callback_key;
        callback_mapper.insert(std::make_pair(
            callback_key,
            std::make_any<BoolCallbackT>(result_callback.value())
        ));
    }

    gupnp_service_proxy_call_action_async(
        handler,
        action,
        NULL,
        set_av_transport_uri_cb_wrapper,
        reinterpret_cast<gpointer>(callback_key)
    );
    gupnp_service_proxy_action_unref(action);
}

void UpnpAVTransportServiceProxy::set_av_transport_uri_cb(GObject *object, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL;
    auto callback_key = reinterpret_cast<uintptr_t>(user_data);
    auto raw_handler = GUPNP_SERVICE_PROXY(object);
    auto action = gupnp_service_proxy_call_action_finish(
        raw_handler, res, &error
    );
    bool result = gupnp_service_proxy_action_get_result(action, &error, NULL);
    g_clear_error (&error);
    g_object_unref(object);
    // this may be wrong service proxy is not the same one
    auto it = s_raw_callback_mapper.find(raw_handler);
    assert(it != s_raw_callback_mapper.cend());
    auto _w = it->second;
    if (_w.expired()) {
        s_raw_callback_mapper.erase(it);
        return;
    }
    auto _this = _w.lock();
    

    if (callback_key == 0) {
        return;
    }
    auto c_it = _this->callback_mapper.find(callback_key);
    if (c_it != _this->callback_mapper.cend())
    {
        std::any_cast<BoolCallbackT>(c_it->second)(result);
        _this->callback_mapper.erase(c_it);
    }
}

static void g_value_free (gpointer data)
{
    g_value_unset ((GValue *) data);
    g_slice_free (GValue, data);
}

void UpnpAVTransportServiceProxy::Control(UpnpAVTransportServiceProxy::ControlType type, const std::optional<UpnpAVTransportServiceProxy::BoolCallbackT> & result_callback)
{
    GList      *names = NULL, *values = NULL;
    auto instance_id = reinterpret_cast<GValue*>(g_slice_alloc0 (sizeof (GValue)));
    g_value_init (instance_id, G_TYPE_UINT);
    g_value_set_uint (instance_id, 0);
    names = g_list_append(names, g_strdup("InstanceID"));
    values = g_list_append(values, instance_id);
    if (strcmp(type.type, ControlType::Play) == 0)
    {
       auto speed = reinterpret_cast<GValue*>(g_slice_alloc0 (sizeof (GValue)));
       g_value_init(speed, G_TYPE_STRING);
       g_value_set_string(speed, "1");
       names = g_list_append(names, g_strdup("Speed"));
       values = g_list_append(values, speed);
    }
    auto action = gupnp_service_proxy_action_new_from_list(
        type.type,
        names,
        values
    );

    uintptr_t callback_key = 0;
    if (result_callback) {
        callback_key = ++last_callback_key;
        callback_mapper.insert(std::make_pair(
            callback_key,
            std::make_any<BoolCallbackT>(result_callback.value())
        ));
    }

    gupnp_service_proxy_call_action_async(
        handler,
        action,
        nullptr,
        av_transport_action_cb_wrapper,
        reinterpret_cast<gpointer>(callback_key)
    );

    gupnp_service_proxy_action_unref(action);
    g_list_free_full(names, g_free);
    g_list_free_full(values, g_value_free);
}

void UpnpAVTransportServiceProxy::av_transport_action_cb(GObject *object, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL;
    bool result = false;
    GUPnPServiceProxy *proxy = GUPNP_SERVICE_PROXY(object);
    auto action = gupnp_service_proxy_call_action_finish(
        proxy,
        res,
        &error
    );
    result = gupnp_service_proxy_action_get_result (action, &error, NULL);
    g_clear_error (&error);
    g_object_unref (object);

    auto it = s_raw_callback_mapper.find(proxy);
    assert(it != s_raw_callback_mapper.cend());

    if (it->second.expired())
    {
        return;
    }

    auto _this = it->second.lock();
    auto callback_key = reinterpret_cast<uintptr_t>(user_data);
    if (0 == callback_key) 
    {
        return;
    }
    auto c_it = _this->callback_mapper.find(callback_key);
    if (c_it != _this->callback_mapper.cend())
    {
        std::any_cast<BoolCallbackT>(c_it->second)(result);
        _this->callback_mapper.erase(c_it);
    }
}
