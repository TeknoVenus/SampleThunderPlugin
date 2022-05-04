project(JSONRPCTestApp)

find_package(${NAMESPACE}Protocols REQUIRED)

# Build simple test app
add_executable(${PROJECT_NAME}
        main.cpp
        Module.cpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ${NAMESPACE}Core::${NAMESPACE}Core
        ${NAMESPACE}Protocols::${NAMESPACE}Protocols
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
)
