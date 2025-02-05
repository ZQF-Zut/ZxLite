# ZxLite
cmake_minimum_required(VERSION 3.25)

# Project Name
project(Zut_ZxLite)

# Source File
list(APPEND "${PROJECT_NAME}_SRC_FILE" 
    "src/Zut/ZxLite/ZIntrinsic.cpp"
    "src/Zut/ZxLite/Console.cpp"
    "src/Zut/ZxLite/MsgBox.cpp"
    "src/Zut/ZxLite/Module.cpp"
    "src/Zut/ZxLite/Injector.cpp"
    "src/Zut/ZxLite/StrView.cpp")

# Set Library
add_library("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC_FILE}")
add_library(Zut::ZxLite ALIAS "${PROJECT_NAME}")
target_include_directories("${PROJECT_NAME}" PUBLIC "src/")
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_23)
target_compile_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/W4>")
target_compile_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
target_link_libraries("${PROJECT_NAME}" PRIVATE phnt::phnt)
