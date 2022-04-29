find_package(${NAMESPACE}Protocols REQUIRED)

# Build simple test app
add_executable(ThunderTestApp
        main.cpp
        Module.cpp
)

target_link_libraries(ThunderTestApp
        PRIVATE
        ${NAMESPACE}Core::${NAMESPACE}Core
        ${NAMESPACE}Protocols::${NAMESPACE}Protocols
)

set_target_properties(ThunderTestApp PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
)
