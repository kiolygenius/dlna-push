#include "UpnpControlPoint.h"

std::map<UpnpControlPoint::RawHandlerPtr, UpnpControlPoint::WPtr> UpnpControlPoint::s_controlpointer_mapper;

UpnpControlPoint::UpnpControlPoint(RawHandlerPtr p)
: handler(p), last_callback_key(0), devices(UpnpDeviceProxy::DefaultComparator())
{
    
}

UpnpControlPoint::~UpnpControlPoint() 
{
    if (handler)
    {
        g_object_unref(handler);
        handler = nullptr;
    }
}

extern "C" {

void raw_device_available_cb_wrapper(UpnpControlPoint::RawHandlerPtr p, UpnpDeviceProxy::RawHandlerPtr dp, void* user_data)
{
    UpnpControlPoint::raw_device_available_cb(p, dp, user_data);
}

void raw_device_unavailable_cb_wrapper(UpnpControlPoint::RawHandlerPtr p, UpnpDeviceProxy::RawHandlerPtr dp, void* user_data) 
{
    UpnpControlPoint::raw_device_unavailable_cb(p, dp, user_data);
}

}

unsigned long UpnpControlPoint::OnDeviceAvailable(const std::function<void(const UpnpControlPoint::SPtr&, const UpnpDeviceProxy::SPtr&)> &cb) 
{
    uintptr_t callback_key = ++last_callback_key;
    callback_mapper.insert(make_pair(callback_key, cb));
    return SignalConnect("device-proxy-available", GCallback(raw_device_available_cb_wrapper), callback_key);
}

unsigned long UpnpControlPoint::OnDeviceUnavailable(const std::function<void(const UpnpControlPoint::SPtr&, const UpnpDeviceProxy::SPtr&)> &cb) 
{
    uintptr_t callback_key = ++last_callback_key;
    callback_mapper.insert(make_pair(callback_key, cb));
    return SignalConnect("device-proxy-unavailable", GCallback(raw_device_unavailable_cb_wrapper), callback_key);
}

unsigned long UpnpControlPoint::SignalConnect(const std::string &signal, GCallback cb, uintptr_t user_data) 
{
    return g_signal_connect(handler, signal.c_str(), cb, reinterpret_cast<void*>(user_data));
}
void UpnpControlPoint::raw_device_available_cb(UpnpControlPoint::RawHandlerPtr p, UpnpDeviceProxy::RawHandlerPtr dp, void* user_data) 
{
    auto it = s_controlpointer_mapper.find(p);
    if (it != s_controlpointer_mapper.cend())
    {
        auto _this = it->second.lock();
        auto pDevice = std::make_shared<UpnpDeviceProxy>(dp);
        _this->devices.insert(pDevice);
        auto cb_it = _this->callback_mapper.find(reinterpret_cast<uintptr_t>(user_data));
        if (cb_it != _this->callback_mapper.cend()) {
            (cb_it->second)(_this, pDevice);
        }
    }
}

void UpnpControlPoint::raw_device_unavailable_cb(RawHandlerPtr p, UpnpDeviceProxy::RawHandlerPtr dp, void* user_data) 
{
    auto it = s_controlpointer_mapper.find(p);
    if (it != s_controlpointer_mapper.cend())
    {
        auto _this = it->second.lock();
        auto comparator = UpnpDeviceProxy::DefaultRawEqualComparator();
        auto device_it = std::find_if(
            _this->devices.cbegin(), 
            _this->devices.cend(), 
            [dp, &comparator](const auto &obj) {
                return comparator(obj, dp); 
            }
        );
        auto pDevice = *device_it;
        _this->devices.erase(device_it);
        auto cb_it = _this->callback_mapper.find(reinterpret_cast<uintptr_t>(user_data));
        if (cb_it != _this->callback_mapper.cend()) {
            (cb_it->second)(_this, pDevice);
        }
    }
}
