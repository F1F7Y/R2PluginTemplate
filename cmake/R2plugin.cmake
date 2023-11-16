## CMake extension for creating R2Northstar V3 Plugins

cmake_minimum_required( VERSION 3.2 )

if(CMAKE_POLICY_DEFAULT_CMP0017 OR CMAKE_POLICY_DEFAULT_CMP0020)
    # touch these to remove warnings
endif()
cmake_policy(SET CMP0057 NEW)

project(R2plugin)

if (NOT WIN32)
    message(FATAL_ERROR "Northstar Plugins can only be compiled for Windows")
endif()

if (__R2PLUGIN_CMAKE_INCLUDED)
    return()
endif()
set(__R2PLUGIN_CMAKE_INCLUDED TRUE)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
include(CheckLinkerFlag)

macro(check_compiler_flags CHECK_FUNC FLAGS OUTPUT)
    foreach(flag ${${FLAGS}})
        string(REPLACE "+" "P" VAR_NAME ${flag})
        string(TOUPPER ${VAR_NAME} VAR_NAME)
        set(flag "-${flag}")
        cmake_language(CALL ${CHECK_FUNC} "${flag}" "${VAR_NAME}")

        if(${${VAR_NAME}})
            list(APPEND ${OUTPUT} "${flag}")
        endif()
    endforeach()
endmacro()

macro(check_linker_flags LANG FLAGS OUTPUT)
    foreach(flag ${${FLAGS}})
        string(REPLACE "+" "P" VAR_NAME ${flag})
        string(TOUPPER ${VAR_NAME} VAR_NAME)
        set(flag "-${flag}")
        check_linker_flag("${LANG}" "${flag}" "${VAR_NAME}")

        if(${${VAR_NAME}})
            list(APPEND ${OUTPUT} "${flag}")
        endif()
    endforeach()
endmacro()


list(APPEND
    C_FLAGS
)

list(APPEND
    C_LINK_FLAGS
    static
    static-libgcc
)

list(APPEND
    CXX_FLAGS
    ${C_FLAGS}
)

list(APPEND
    CXX_LINK_FLAGS
    ${C_LINK_FLAGS}
    static-libstdc++
)


get_property(languages GLOBAL PROPERTY ENABLED_LANGUAGES)

if("C" IN_LIST languages)
    check_compiler_flags(check_c_compiler_flag C_FLAGS C_FLAGS_LIST)
    list(JOIN C_FLAGS_LIST " " C_FLAGS)
    set(PLUGIN_C_FLAGS "${PLUGIN_C_FLAGS} ${C_FLAGS}")

    check_linker_flags("C" C_LINK_FLAGS C_LINK_FLAGS_LIST)
    list(JOIN C_LINK_FLAGS_LIST " " PLUGIN_C_LINK_FLAGS)
endif()
if("CXX" IN_LIST languages)
    check_compiler_flags(check_cxx_compiler_flag CXX_FLAGS CXX_FLAGS_LIST)
    list(JOIN CXX_FLAGS_LIST " " CXX_FLAGS)
    set(PLUGIN_CXX_FLAGS "${PLUGIN_CXX_FLAGS} ${CXX_FLAGS}")

    check_linker_flags("C" CXX_LINK_FLAGS CXX_LINK_FLAGS_LIST)
    list(JOIN CXX_LINK_FLAGS_LIST " " PLUGIN_CXX_LINK_FLAGS)
endif()

set(RESOURCE_TEMPLATE
"
#define IDR_RCDATA1 101
IDR_RCDATA1             RCDATA                  \"manifest.json\"

VS_VERSION_INFO VERSIONINFO
 FILEVERSION 1,0,0,0
 PRODUCTVERSION 1,0,0,0
 FILEFLAGSMASK 0x3fL
 FILEFLAGS 0x0L
 FILEOS 0x40004L
 FILETYPE 0x2L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK \"StringFileInfo\"
    BEGIN
        BLOCK \"040904b0\"
        BEGIN
            VALUE \"CompanyName\", \"Northstar\"
            VALUE \"FileDescription\", \"Northstar Plugin\"
            VALUE \"FileVersion\", \"1.0.0.0\"
            VALUE \"ProductName\", \"Northstar Plugin\"
            VALUE \"ProductVersion\", \"1.0.0.0\"
        END
    END
    BLOCK \"VarFileInfo\"
    BEGIN
        VALUE \"Translation\", 0x409, 1200
    END
END
")

set(MANIFEST_TEMPLATE
    "{
        \"description\": \"\",
        \"api_version\": \"3\",
        \"version\": \"0\",
        \"run_on_server\": false,
        \"run_on_client\": false
    }"
)

list(APPEND
    REQUIRED_MANIFEST_KEYS
    name
    displayname
    description
    api_version
    version
    run_on_server
    run_on_client
)
macro(plugin_manifest TARGET KEY VALUE)
    if (NOT ${TARGET}_MANIFEST)
        set(${TARGET}_MANIFEST "${MANIFEST_TEMPLATE}")

        # set default name to target name
        string(JSON ${TARGET}_MANIFEST SET "${${TARGET}_MANIFEST}" name "\"${TARGET}\"")

        # set default version to project version
        string(JSON ${TARGET}_MANIFEST SET "${${TARGET}_MANIFEST}" version "\"${CMAKE_PROJECT_VERSION}\"")
    endif()

    if ("${VALUE}" STREQUAL ON)
        set(JSON_VALUE "true")
    elseif ("${VALUE}" STREQUAL OFF)
        set(JSON_VALUE "false")
    else()
        set(JSON_VALUE "\"${VALUE}\"")
    endif()

    string(JSON ${TARGET}_MANIFEST SET "${${TARGET}_MANIFEST}" "${KEY}" "${JSON_VALUE}")
endmacro()

macro(plugin_link TARGET)
    if("C" IN_LIST languages)
        target_link_libraries(${TARGET} ${PLUGIN_C_LINK_FLAGS})
    endif()
    if("CXX" IN_LIST languages)
        target_link_libraries(${TARGET} ${PLUGIN_CXX_LINK_FLAGS})
    endif()

    set(MANIFEST_DIR "${CMAKE_BINARY_DIR}/${TARGET}_plugin/")
    file(MAKE_DIRECTORY "${MANIFEST_DIR}")
    file(WRITE "${MANIFEST_DIR}/manifest.json" "${${TARGET}_MANIFEST}")
    file(WRITE "${MANIFEST_DIR}/manifest.rc" "${RESOURCE_TEMPLATE}")
    target_sources(${TARGET} PUBLIC "${MANIFEST_DIR}/manifest.rc")
endmacro()
