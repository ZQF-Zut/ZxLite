# OutPath
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x32-${CMAKE_BUILD_TYPE}/")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x32-${CMAKE_BUILD_TYPE}/")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x32-${CMAKE_BUILD_TYPE}/lib/")
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x64-${CMAKE_BUILD_TYPE}/")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x64-${CMAKE_BUILD_TYPE}/")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x64-${CMAKE_BUILD_TYPE}/lib/")
endif()

# Link
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    # LTCG
    cmake_policy(SET CMP0069 NEW) 
    set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPO_CHECK_RESULT OUTPUT output)
    if(IPO_CHECK_RESULT)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    else()
        message(WARNING "IPO is not supported: ${output}")
    endif()

    # Static Library
    set(BUILD_SHARED_LIBS OFF)

    # Static Runtime
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded)
    endif()
else()
    set(BUILD_SHARED_LIBS ON)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Compiler
if(MSVC)
    add_compile_definitions(WIN32_LEAN_AND_MEAN UNICODE _UNICODE)
    string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    add_link_options(/MERGE:.data=.text)
    add_link_options(/MERGE:.rdata=.text)
    add_link_options(/MERGE:.pdata=.text)
    # add_link_options(/MERGE:.idata=.text)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Gy)
        add_compile_options(/Zc:inline)
        add_compile_options(/sdl-)
        add_compile_options(/GS-)
        add_compile_options(/GR-)
        add_link_options(/nologo /NODEFAULTLIB /MANIFEST:NO)
    endif()
endif()
