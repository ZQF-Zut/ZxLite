# ZxLite Main
cmake_minimum_required(VERSION 3.31)

# VCPKG
if(ZQF_VCPKG)
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/vcpkg_fetch.cmake")
    file(DOWNLOAD https://raw.githubusercontent.com/Dir-A/Registry/a30058c632ccf7530627f39eff624d495444d676/cmake/vcpkg_fetch.cmake "${CMAKE_BINARY_DIR}/vcpkg_fetch.cmake")
  endif()

  include("${CMAKE_BINARY_DIR}/vcpkg_fetch.cmake")
endif()

# Project Name
project(zut_zxlite_main)

# Modules
find_package(zqf_cmake_modules CONFIG REQUIRED)

# Config
include("zqf_config")
zqf_config_cxx_exception(DisableGlobally)
zqf_config_runtime_error_checks(DisableGlobally)

# Config Dev
if(ZQF_DEVELOP)
  include("zqf_config")
  include("zqf_profiles")
  zqf_config_output_dir()
  zqf_profiles_fetch(CLANGD EDITOR_CONFIG)
endif()

# Source
add_subdirectory("src")

# Dev
if(ZQF_DEVELOP)
  add_subdirectory("tryout")
endif()

# Testing
if(ZQF_TESTING)
  enable_testing()
  add_subdirectory("test")
endif()
