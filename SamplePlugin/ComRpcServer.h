/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2022 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#pragma once

#include "Module.h"
#include <interfaces/ISamplePlugin.h>

using namespace WPEFramework;

/**
 * @brief A custom COM-RPC server implementation for a private connection to our plugin
 * instead of going via Thunder communicator
*/
class ComRpcServer : public RPC::Communicator {
public:
    ComRpcServer() = delete;
    ComRpcServer(const ComRpcServer&) = delete;
    ComRpcServer& operator=(const ComRpcServer&) = delete;

    ComRpcServer(const Core::NodeId& socket,
        Exchange::ISamplePlugin* parentInterface,
        PluginHost::IShell* shell,
        const string& proxyStubPath,
        const Core::ProxyType<RPC::InvokeServer>& engine);

    ~ComRpcServer();

private:
    // If a client wants to acquire an interface, provide it to them
    void* Acquire(const string& className, const uint32_t interfaceId, const uint32_t versionId) override;

private:
    Exchange::ISamplePlugin* _samplePlugin;
    PluginHost::IShell* _shell;
};