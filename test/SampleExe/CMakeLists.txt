# Project Name
project(SampleExe)

# Current Project
add_executable("${PROJECT_NAME}" "main.cpp")
# set_target_properties("${PROJECT_NAME}" PROPERTIES WIN32_EXECUTABLE ON)
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_23)
target_compile_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/W4>")
target_compile_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
target_link_options("${PROJECT_NAME}" PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/ENTRY:Start>")

# Add Library
target_link_libraries("${PROJECT_NAME}" PRIVATE Zut::ZxLite phnt::phnt)
