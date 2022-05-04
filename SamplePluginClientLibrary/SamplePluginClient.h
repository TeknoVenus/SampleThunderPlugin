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

        // Must define an interface map since we are implementing an interface on the exchange
        // so Thunder knows what type we are
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

    // The implementation of the ISamplePlugin interface - this could be an in-process, out-of-process
    // or distributed plugin, but as a client we don't need to worry about that
    Exchange::ISamplePlugin *mSamplePlugin;
    PluginHost::IShell *mController;

    bool mValid;

    // Instance of our notification handler
    Core::Sink<NotificationHandler> mNotification;
};