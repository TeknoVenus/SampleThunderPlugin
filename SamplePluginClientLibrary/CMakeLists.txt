find_package(${NAMESPACE}Protocols REQUIRED)

# Build client library
add_library(SamplePluginClientLib SHARED
        SamplePluginClient.cpp
        Module.cpp
)

set_target_properties(SamplePluginClientLib PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
)

target_link_libraries(SamplePluginClientLib
        ${NAMESPACE}Core::${NAMESPACE}Core
        ${NAMESPACE}Protocols::${NAMESPACE}Protocols
)

# Build simple test app to exercise client library
add_executable(SamplePluginClient
        Module.cpp
        main.cpp
)

# Only need to link to client lib, will bring in dependencies
# automatically
target_link_libraries(SamplePluginClient
        PRIVATE
        SamplePluginClientLib
)

set_target_properties(SamplePluginClient PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
)
