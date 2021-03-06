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

#include <mutex>

#include "Module.h"

#include <interfaces/ISamplePlugin.h>


namespace WPEFramework
{
    namespace Plugin
    {
        class SamplePluginImplementation : public Exchange::ISamplePlugin
        {
        public:
            SamplePluginImplementation();
            ~SamplePluginImplementation() override;

            // Do not allow copy/move constructors
            SamplePluginImplementation(const SamplePluginImplementation &) = delete;
            SamplePluginImplementation &operator=(const SamplePluginImplementation &) = delete;

            BEGIN_INTERFACE_MAP(SamplePluginImplementation)
            INTERFACE_ENTRY(Exchange::ISamplePlugin)
            END_INTERFACE_MAP

        public:
            // Implement the main methods from ISamplePlugin
            uint32_t Greet(const string &message, string &result /* @out */) override;
            uint32_t Echo(const string &message, string &result /* @out */) override;

        public:
            // Handle Notification registration/removal
            uint32_t Register(ISamplePlugin::INotification *notification) override;
            uint32_t Unregister(ISamplePlugin::INotification *notification) override;

        private:
            std::list<Exchange::ISamplePlugin::INotification *> _notificationCallbacks;
            std::mutex _notificationMutex;

            const std::vector<std::string> _greetings;
        };
    }
}