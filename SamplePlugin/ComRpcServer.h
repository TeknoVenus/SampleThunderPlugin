#pragma once

#include "Module.h"
#include <interfaces/ISamplePlugin.h>

using namespace WPEFramework;

/**
 * A custom COM-RPC server implementation for a private connection to our plugin
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
    void* Aquire(const string& className, const uint32_t interfaceId, const uint32_t versionId) override;

private:
    Exchange::ISamplePlugin* _samplePlugin;
    PluginHost::IShell* _shell;
};