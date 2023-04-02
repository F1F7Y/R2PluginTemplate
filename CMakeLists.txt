cmake_minimum_required(VERSION 3.21.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(R2plugin)

project(CPlugTemplate VERSION 1.0)

# Build settings
set(CMAKE_BUILD_TYPE Release)

# CXX settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-fpermissive")

# No "lib" prefix :)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

# Find SPDLOG
# include(FindPkgConfig)
# pkg_check_modules(SPDLOG spdlog REQUIRED )
find_package( spdlog REQUIRED )
find_package( fmt REQUIRED )


include_directories(. src src/lib include)

add_library(CPlugTest SHARED
            src/lib/bits.cpp
            src/lib/color.cpp
            src/lib/concommand.cpp
            src/lib/convar.cpp
            src/lib/loader.cpp
            src/lib/plugin_lib.cpp
            src/lib/squirrel.cpp
            src/lib/squirrelautobind.cpp
            src/plugin.cpp
           )

plugin_manifest(CPlugTest name "cplug-test")
plugin_manifest(CPlugTest displayname "CPLUGTEST")
plugin_manifest(CPlugTest description "gcc plug testing")
plugin_manifest(CPlugTest run_on_server ON)
#plugin_manifest(CPlugTest run_on_client OFF)

target_precompile_headers(CPlugTest PUBLIC src/pch.h)

plugin_link(CPlugTest)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(CPlugTest
                      spdlog::spdlog_header_only
                      fmt::fmt-header-only
                      Threads::Threads
                     )
