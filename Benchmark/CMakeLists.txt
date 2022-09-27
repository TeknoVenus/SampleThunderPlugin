# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
#
# Copyright 2022 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
project(Benchmark)

find_package(benchmark REQUIRED)

find_package(WPEFrameworkCOM REQUIRED)
find_package(WPEFrameworkCore REQUIRED)
find_package(WPEFrameworkCryptalgo REQUIRED)
find_package(WPEFrameworkWebSocket REQUIRED)
find_package(WPEFrameworkMessaging REQUIRED)


add_executable(${PROJECT_NAME}
    main.cpp
    comrpc.cpp
    jsonrpc.cpp
    messages.cpp
    Module.cpp
)

target_link_libraries(${PROJECT_NAME}
    benchmark::benchmark
    WPEFrameworkCore::WPEFrameworkCore
    WPEFrameworkCOM::WPEFrameworkCOM
    WPEFrameworkCryptalgo::WPEFrameworkCryptalgo
    WPEFrameworkWebSocket::WPEFrameworkWebSocket
    WPEFrameworkMessaging::WPEFrameworkMessaging
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
)
