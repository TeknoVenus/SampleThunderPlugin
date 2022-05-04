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