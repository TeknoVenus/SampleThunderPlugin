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
#include "SamplePluginConfiguration.h"

#include <interfaces/ISamplePlugin.h>
#include <random>

namespace WPEFramework {
namespace Plugin {
    class SamplePluginImplementation : public Exchange::ISamplePlugin {
    public:
        SamplePluginImplementation();
        ~SamplePluginImplementation() override;

        // Do not allow copy/move constructors
        SamplePluginImplementation(const SamplePluginImplementation&) = delete;
        SamplePluginImplementation& operator=(const SamplePluginImplementation&) = delete;

        BEGIN_INTERFACE_MAP(SamplePluginImplementation)
        INTERFACE_ENTRY(Exchange::ISamplePlugin)
        END_INTERFACE_MAP

    public:
        // Implement the methods from ISamplePlugin
        uint32_t Greet(const string& message, string& result) override;
        uint32_t Echo(const string& message, string& result) override;
        uint32_t Configure(PluginHost::IShell* framework) override;

        // Handle graceful notification registration/removal by clients
        uint32_t Register(ISamplePlugin::INotification* notification) override;
        uint32_t Unregister(ISamplePlugin::INotification* notification) override;

    private:
        // Utility method to send a notification to all interested clients
        void RaiseSomethingHappenedNotification(const INotification::Source& eventSource);

    private:
        std::list<Exchange::ISamplePlugin::INotification*> _notificationCallbacks;
        SamplePluginConfiguration _config;

        // By using Thunder's CriticalSection instead of std::mutex, we can get some
        // useful warning reporting for free for deadlock detection and long locks
        Core::CriticalSection _notificationMutex;

        std::vector<std::string> _greetings;

        std::random_device _randomDevice;
        std::mt19937 _rng;
    };
}
}