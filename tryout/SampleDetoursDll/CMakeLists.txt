# Project
project(zut_zxlite_sample_detours_dll VERSION 1.0 LANGUAGES C CXX)

# Module
add_library(${PROJECT_NAME} MODULE "dllmain.cpp")

# Compiler
include("zqf_target")
zqf_target_encoding(${PROJECT_NAME} PRIVATE UTF8)
zqf_target_warning(${PROJECT_NAME} PRIVATE AllExtra)
zqf_target_manifest(${PROJECT_NAME} PRIVATE OFF)
zqf_target_default_lib(${PROJECT_NAME} PRIVATE OFF)
zqf_target_nocrt_flags(${PROJECT_NAME} PRIVATE All)
zqf_target_entrypoint(${PROJECT_NAME} PRIVATE CustomEntry "Start")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# Properties
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)

# External Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Zut::ZxLite Zut::ZxLite::Intrinsic Detours::Detours)
