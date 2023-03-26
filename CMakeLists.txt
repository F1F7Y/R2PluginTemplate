cmake_minimum_required(VERSION 3.21.1)

project(CPlugTemplate)

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
            manifest/resource.h
            manifest/resource.rc
           )

target_precompile_headers(CPlugTest PUBLIC src/pch.h)

target_link_libraries(CPlugTest
                      -static-libgcc
                      -static-libstdc++
                      spdlog::spdlog_header_only
                      fmt::fmt-header-only
                      -static -lpthread
                     )
