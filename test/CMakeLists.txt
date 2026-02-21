# Project Name
project(zut_zxlite_test VERSION 1.0 LANGUAGES C CXX)

# Current Project
add_executable(${PROJECT_NAME} "main.cpp")

# Compiler
include("zqf_target")
zqf_target_encoding(${PROJECT_NAME} PRIVATE UTF8)
zqf_target_warning(${PROJECT_NAME} PRIVATE AllExtra)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# External Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Zut::ZxLite)

# Testing
find_package(Catch2 CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
include(Catch)
catch_discover_tests(${PROJECT_NAME})
