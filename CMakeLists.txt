cmake_minimum_required(VERSION 3.12)

project(dlna-push VERSION 1.0.0)

set(LIB_SRC src/upnp++/UpnpResourceFactory.cpp)

set(SRC src/main.cpp
        ${LIB_SRC})

find_package(PkgConfig)
pkg_search_module(gupnp REQUIRED glib-2.0 gupnp-1.2 gupnp-av-1.0)

add_executable(dlna-cast ${SRC})

target_include_directories(dlna-cast PUBLIC ${gupnp_INCLUDE_DIRS})
target_link_libraries(dlna-cast ${gupnp_LIBRARIES})
