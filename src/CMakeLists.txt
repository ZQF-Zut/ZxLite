# Project
project(zut_zxlite VERSION 1.0 LANGUAGES C CXX)

# Library
add_library(${PROJECT_NAME} STATIC
  "src/Zut/ZxLite/Console.cpp"
  "src/Zut/ZxLite/MsgBox.cpp"
  "src/Zut/ZxLite/Module.cpp"
  "src/Zut/ZxLite/Hasher.cpp"
  "src/Zut/ZxLite/Injector.cpp"
  "src/Zut/ZxLite/String.cpp"
)
add_library(Zut::ZxLite ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src/> $<INSTALL_INTERFACE:include>)

# Compiler
include("zqf_target")
zqf_target_encoding(${PROJECT_NAME} PRIVATE UTF8)
zqf_target_warning(${PROJECT_NAME} PRIVATE AllExtra)
zqf_target_nocrt_flags(${PROJECT_NAME} PRIVATE All)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

# Intrinsic
add_library(${PROJECT_NAME}_intrinsic STATIC "src/Zut/ZxLite/ZIntrinsic.cpp")
add_library(Zut::ZxLite::Intrinsic ALIAS ${PROJECT_NAME}_intrinsic)
zqf_target_nocrt_flags(${PROJECT_NAME}_intrinsic PRIVATE All)

# External Libraries
find_package(phnt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC phnt::phnt)
