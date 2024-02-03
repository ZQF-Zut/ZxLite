# Project Name
project(SampleDll)

# Win32
set(CMAKE_WIN32_EXECUTABLE ON)

# Cpp Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Current Project
add_library("${PROJECT_NAME}" MODULE "dllmain.cpp")

# Add Library
target_link_libraries("${PROJECT_NAME}" PRIVATE RxLite)
target_link_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/ENTRY:DllEntryPoint>")

