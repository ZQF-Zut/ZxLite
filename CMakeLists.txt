# Main CMakeLists
cmake_minimum_required(VERSION 3.15)

# Project Name
project(RxLite)

# Cpp Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set Library
add_library("${PROJECT_NAME}" "src/RxLite/RxLite.cpp")
target_include_directories("${PROJECT_NAME}" INTERFACE "src/")

if(NOT USER_CONFIG_RXLITE_BUILD_TEST)
    return()
endif()

# Option
set(USER_CONFIG_BUILD_TEST TRUE)
set(USER_CONFIG_IPO_OPTIMIZATION TRUE)
set(USER_CONFIG_STATIC_MSVC_RUNTIME TRUE)

# Value Define
set(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}")

# Out Path
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(EXECUTABLE_OUTPUT_PATH "${ROOT_DIR}/.bin/x32")
    set(LIBRARY_OUTPUT_PATH "${ROOT_DIR}/.bin/x32")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(EXECUTABLE_OUTPUT_PATH "${ROOT_DIR}/.bin/x64")
    set(LIBRARY_OUTPUT_PATH "${ROOT_DIR}/.bin/x64")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/debug")
    set(LIBRARY_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH}/debug")
else()
    set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/release")
    set(LIBRARY_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH}/release")
endif()

# IPO
if(USER_CONFIG_IPO_OPTIMIZATION)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_check_result OUTPUT output)
    if(ipo_check_result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL TRUE)
    else()
        message(WARNING "IPO is not supported: ${output}")
    endif()
endif()

# MSVC Setting
if(MSVC)
    # Runtime
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            if (USER_CONFIG_STATIC_MSVC_RUNTIME)
                set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebug)
            else()
                set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDebugDLL)
            endif()
        else()
            if (USER_CONFIG_STATIC_MSVC_RUNTIME)
                set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
            else()
                set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
            endif()
        endif()

    # Macro
        add_definitions(-DUNICODE -D_UNICODE)

    # Compile Flags
        add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")

        # I know that it makes all the compile flags change, But I just can't find a way to turn off the specified compile flag for a specific target.
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t /Zc:inline /Zc:forScope")
        string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REPLACE "/GS" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        else()
            add_link_options(/OPT:REF)
            set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Gy")
        endif()
endif()

# Include
include_directories("${ROOT_DIR}")

# Library
add_subdirectory("test")