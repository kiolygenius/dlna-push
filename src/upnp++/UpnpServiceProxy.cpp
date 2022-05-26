#include "UpnpServiceProxy.h"

UpnpServiceProxy::UpnpServiceProxy(UpnpServiceProxy::RawhandlerPtr p) 
: handler(p)
{
    
}

UpnpServiceProxy::~UpnpServiceProxy() 
{
    g_object_unref(handler);
}
