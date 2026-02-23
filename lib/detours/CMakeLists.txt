project(Detours_Static VERSION 1.0 LANGUAGES C CXX)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
  message(FATAL_ERROR "detours windows only")
endif()

# Detours
include(FetchContent)
FetchContent_Declare(
  Detours
  GIT_REPOSITORY https://github.com/microsoft/Detours.git
  GIT_TAG v4.0.1
  GIT_SHALLOW TRUE
  GIT_PROGRESSTRUE
)
FetchContent_MakeAvailable(Detours)

if(NOT detours_POPULATED)
  message(FATAL_ERROR "fetch detours source code error")
endif()

add_library(${PROJECT_NAME} STATIC
  "${detours_SOURCE_DIR}/src/creatwth.cpp"
  "${detours_SOURCE_DIR}/src/detours.cpp"
  "${detours_SOURCE_DIR}/src/disasm.cpp"
  "${detours_SOURCE_DIR}/src/disolarm64.cpp"
  "${detours_SOURCE_DIR}/src/disolx64.cpp"
  "${detours_SOURCE_DIR}/src/disolx86.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/Mock.cpp"
)
add_library(Detours::Detours ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE "${detours_SOURCE_DIR}/src")

# Compiler
include("zqf_target")
zqf_target_nocrt_flags(${PROJECT_NAME} PRIVATE All)
target_compile_options(${PROJECT_NAME} PRIVATE /FI${CMAKE_CURRENT_LIST_DIR}/Mock.h)

# External Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Zut::ZxLite::Intrinsic)
target_link_libraries(${PROJECT_NAME} PRIVATE $<$<PLATFORM_ID:Windows>:ntdll.lib>)
