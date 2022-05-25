#include "UpnpResourceFactory.h"

UpnpResourceFactory::UpnpResourceFactory() 
: factory(gupnp_resource_factory_get_default())
{

}

UpnpResourceFactory::~UpnpResourceFactory()
{
    
}

GUPnPResourceFactory *UpnpResourceFactory::getRawHandler() const
{
    return factory;
}

