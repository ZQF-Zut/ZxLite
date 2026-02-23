# Project
project(zut_zxlite_intrinsic VERSION 1.0 LANGUAGES C CXX)

# Intrinsic
add_library(${PROJECT_NAME} STATIC "intrinsic.cpp")
add_library(Zut::ZxLite::Intrinsic ALIAS ${PROJECT_NAME})

# Compiler
include("zqf_target")
zqf_target_nocrt_flags(${PROJECT_NAME} PRIVATE All)
