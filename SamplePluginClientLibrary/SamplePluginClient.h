#pragma once

#include "Module.h"
#include <interfaces/ISamplePlugin.h>

#include <string>

// Some logging boilerplate
#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define Log(fmt, ...)                                                                                   \
    do                                                                                                  \
    {                                                                                                   \
        fprintf(stderr, "[%s:%d](%s): " fmt "\n", __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        fflush(stderr);                                                                                 \
    } while (0)
// End logging boilerplate

using namespace WPEFramework;

class SamplePluginClient
{
    // We want do run our own custom code when the plugin raises a notification
    // Implement the INotification class to do what we want
    class NotificationHandler : public Exchange::ISamplePlugin::INotification
    {
        void SomethingHappend(const Source event) override;

        // Must define an interface map so Thunder knows what type we are when
        // we register ourselves
        BEGIN_INTERFACE_MAP(NotificationHandler)
        INTERFACE_ENTRY(Exchange::ISamplePlugin::INotification)
        END_INTERFACE_MAP
    };

public:
    SamplePluginClient();
    ~SamplePluginClient();

public:
    bool ActivateSamplePlugin();
    bool DeactivateSamplePlugin();

    std::string GetGreeting(const std::string &message);
    bool IsValid();

private:
    Core::NodeId GetConnectionEndpoint();

private:
    Core::NodeId mRemoteConnection;

    // An engine that can serialize/deserialize the COMRPC messages. Can be configured
    // to tune performance:
    // 1 = Number of threads allocated to this connection
    // 0 = Stack size per thread
    // 4 = Message slots. 4 which means that if 4 messages have
    // been queued, the submission of the 5th element will be a
    // blocking call until there is a free slot again
    Core::ProxyType<RPC::InvokeServerType<1, 0, 4>> mEngine;

    Core::ProxyType<RPC::CommunicatorClient> mClient;

    Exchange::ISamplePlugin *mSamplePlugin;
    PluginHost::IShell *mController;

    bool mValid;

    // Instance of our notification handler
    Core::Sink<NotificationHandler> mNotification;
};