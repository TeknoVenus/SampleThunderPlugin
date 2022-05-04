project(COMRPCTestApp)

# Build simple test app to exercise client library
add_executable(${PROJECT_NAME}
        Module.cpp
        main.cpp
)

# Only need to link to client lib, will bring in dependencies automatically
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        SamplePluginClientLib
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
)