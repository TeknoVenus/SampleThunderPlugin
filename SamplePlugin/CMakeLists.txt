set(PLUGIN_NAME SamplePlugin)
set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})
set(PLUGIN_IMPLEMENTATION ${MODULE_NAME}Implementation)

find_package(${NAMESPACE}Plugins REQUIRED)

# Build two libraries to make it obvious what is running in and out of process
# https://wiki.rdkcentral.com/pages/viewpage.action?pageId=198262351

# Build the main plugin that runs inside the WPEFramework daemon
add_library(${MODULE_NAME} SHARED
        SamplePlugin.cpp
        SamplePluginJsonRpc.cpp
        Module.cpp
)

target_link_libraries(${MODULE_NAME}
        PRIVATE
        ${NAMESPACE}Plugins::${NAMESPACE}Plugins
)

install(TARGETS
        ${MODULE_NAME}
        DESTINATION
        lib/${STORAGE_DIRECTORY}/plugins
)


# Build the implementation that runs out-of-process behind a COM-RPC interface
add_library(${PLUGIN_IMPLEMENTATION} SHARED
        SamplePluginImplementation.cpp
        Module.cpp
)

target_link_libraries(${PLUGIN_IMPLEMENTATION}
        PRIVATE
        ${NAMESPACE}Plugins::${NAMESPACE}Plugins
)

install(TARGETS
        ${PLUGIN_IMPLEMENTATION}
        DESTINATION
        lib/${STORAGE_DIRECTORY}/plugins)


write_config(${PLUGIN_NAME})
