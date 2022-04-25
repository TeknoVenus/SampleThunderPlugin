#pragma once

#include "Module.h"
#include <interfaces/ISamplePlugin.h>

#include <string>


#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define Log(fmt, ...)                                                                        \
    do                                                                                             \
    {                                                                                              \
        fprintf(stderr, "[%s:%d](%s): " fmt "\n", __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        fflush(stderr);                                                                            \
    } while (0)

using namespace WPEFramework;

class SamplePluginClient
{
public:
    SamplePluginClient();
    ~SamplePluginClient();

public:
    std::string GetGreeting(const std::string &message);

private:
    Core::NodeId GetConnectionEndpoint();

private:
    Core::NodeId mRemoteConnection;

    // 1 = Thread pool size
    // 0 = Stack size per thread
    // 4 = Message slots
    Core::ProxyType<RPC::InvokeServerType<1, 0, 4>> mEngine;
    Core::ProxyType<RPC::CommunicatorClient> mClient;

    Exchange::ISamplePlugin* mSamplePlugin;
    PluginHost::IShell* mController;

    bool mValid;
};