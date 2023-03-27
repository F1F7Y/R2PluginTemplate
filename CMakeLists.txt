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
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)


include_directories(. plugin lib)

add_library(CPlugTest SHARED
            lib/bits.cpp
            lib/color.cpp
            lib/concommand.cpp
            lib/convar.cpp
            lib/loader.cpp
            lib/plugin_lib.cpp
            lib/squirrel.cpp
            lib/squirrelautobind.cpp
            plugin/plugin.cpp
            manifest/resource.h
            manifest/manifest.rc
           )

target_precompile_headers(CPlugTest PUBLIC src/pch.h)

target_link_libraries(CPlugTest
                    #   -static-libgcc
                    #   -static-libstdc++
                      spdlog::spdlog_header_only
                      fmt::fmt-header-only
                      -static -lpthread
                     )
