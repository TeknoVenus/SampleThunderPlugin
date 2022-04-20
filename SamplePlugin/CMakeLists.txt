set(PLUGIN_NAME SamplePlugin)
set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})

find_package(${NAMESPACE}Plugins REQUIRED)

add_library(${MODULE_NAME} SHARED
        SamplePlugin.cpp
        SamplePluginImplementation.cpp
        SamplePluginJsonRpc.cpp
        Module.cpp
)

target_link_libraries(${MODULE_NAME}
        PRIVATE
        ${NAMESPACE}Plugins::${NAMESPACE}Plugins
)

# ${NAMESPACE}Protocols::${NAMESPACE}Protocols
install(TARGETS
        ${MODULE_NAME}
        DESTINATION
        lib/${STORAGE_DIRECTORY}/plugins)

write_config(${PLUGIN_NAME})
