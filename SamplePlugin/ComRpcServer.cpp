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

#include "ComRpcServer.h"

/**
 * @brief Construct a custom COM-RPC server that exposes a specific plugin interface
 *
 * Allows clients to connect directly to SamplePlugin instead of routing request through WPEFramework daemon. This can
 * allow for better performance in some scenarios, or increased security
*/
ComRpcServer::ComRpcServer(const Core::NodeId& socket,
    Exchange::ISamplePlugin* parentInterface,
    PluginHost::IShell* shell,
    const string& proxyStubPath,
    const Core::ProxyType<RPC::InvokeServer>& engine)
    : RPC::Communicator(socket, proxyStubPath, Core::ProxyType<Core::IIPCServer>(engine))
    , _samplePlugin(parentInterface)
    , _shell(shell)
{
    uint32_t result = Open(Core::infinite);

    engine->Announcements(RPC::Communicator::Announcement());

    if (result != Core::ERROR_NONE) {
        TRACE(Trace::Error, (_T("Failed to open COM-RPC server for SamplePlugin with error %d (%s)"), result, Core::ErrorToString(result)));
    } else {
        TRACE(Trace::Initialisation, (_T("Successfully opened COM-RPC server for SamplePlugin @ '%s'"), RPC::Communicator::Connector().c_str()));
    }
}

ComRpcServer::~ComRpcServer()
{
    TRACE(Trace::Information, (_T("Shutting down COM-RPC server for SamplePlugin")));

    Close(Core::infinite);
}

/**
 * @brief If the connecting client wants an interface we provide, return it to them
 *
 * We can only return either the ISamplePlugin or IShell interfaces provided to us in the
 * constructor since we don't know about anything else
*/
void* ComRpcServer::Acquire(const string& className VARIABLE_IS_NOT_USED, const uint32_t interfaceId,
    const uint32_t versionId)
{
    void* result = nullptr;

    // Currently we only support version 1
    if ((versionId == 1) || (versionId == static_cast<uint32_t>(~0))) {
        if (interfaceId == Exchange::ISamplePlugin::ID && _samplePlugin != nullptr) {
            result = _samplePlugin->QueryInterface(interfaceId);
        } else if (interfaceId == PluginHost::IShell::ID && _shell != nullptr) {
            result = _shell->QueryInterface(interfaceId);
        } else {
            TRACE(Trace::Error, (_T("Can only acquire the ISamplePlugin or IShell interface from this server! (Attempted to acquire interface ID %d)"), interfaceId));
        }
    }

    return result;
}