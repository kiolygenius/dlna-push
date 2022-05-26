cmake_minimum_required(VERSION 3.12)

project(dlna-push VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

set(LIB_SRC 
        src/upnp++/UpnpResourceFactory.cpp
        src/upnp++/UpnpContextManager.cpp
        src/upnp++/UpnpContext.cpp
        src/upnp++/UpnpControlPoint.cpp
        src/upnp++/UpnpDeviceProxy.cpp
        src/upnp++/UpnpServiceProxy.cpp
        src/upnp++/UpnpAVTransportServiceProxy.cpp
        src/upnp++/UpnpDeviceInfo.cpp)

set(SRC src/main.cpp
        src/PushService.cpp
        ${LIB_SRC})

find_package(PkgConfig)
pkg_check_modules(gupnp REQUIRED glib-2.0 gupnp-1.2 gupnp-av-1.0)

add_executable(dlna-cast ${SRC})

target_include_directories(dlna-cast PUBLIC ${gupnp_INCLUDE_DIRS})
target_link_libraries(dlna-cast ${gupnp_LIBRARIES})
