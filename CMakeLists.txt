cmake_minimum_required(VERSION 3.3)

find_package(WPEFramework)

include(CmakeHelperFunctions)

string(TOLOWER ${NAMESPACE} STORAGE_DIRECTORY)

get_directory_property(SEVICES_DEFINES COMPILE_DEFINITIONS)

add_subdirectory(SamplePlugin)