cmake_minimum_required(VERSION 3.3)

project(SampleThunderPlugin)

find_package(WPEFramework)

include(CmakeHelperFunctions)

string(TOLOWER ${NAMESPACE} STORAGE_DIRECTORY)

get_directory_property(SEVICES_DEFINES COMPILE_DEFINITIONS)

add_subdirectory(SamplePlugin)
add_subdirectory(SamplePluginClientLibrary)
add_subdirectory(TestApps/COMRPC)
add_subdirectory(TestApps/JSONRPC)