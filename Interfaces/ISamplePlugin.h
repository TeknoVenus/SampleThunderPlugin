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

namespace WPEFramework
{
    namespace Exchange
    {
        /*
            This is the COM-RPC interface for the sample plugin

            The requirement to make an interface a COMRPC interface is that it should inherit
            from Core::IUnknown
        */
        struct EXTERNAL ISamplePlugin : virtual public Core::IUnknown
        {
            // All interfaces require a unique ID, defined in Ids.h
            enum
            {
                ID = ID_SAMPLE_PLUGIN
            };

            // Define the COM-RPC methods to be implemented by the plugin

            /* @brief Return a greeting */
            virtual uint32_t Greet(const string &message,
                                     string &result /* @out */) = 0;

            virtual uint32_t Echo(const string &message, string &result /* @out */) = 0;

            /* @event */
            struct EXTERNAL INotification : virtual public Core::IUnknown
            {
                enum
                {
                    ID = ID_SAMPLE_PLUGIN_NOTIFICATION
                };

                // The events the plugin can raise
                enum Source : uint8_t
                {
                    EXCITING_THING_HAPPENED,
                    BORING_THING_HAPPENED
                };

                // Notifications that other processes can subscribe to
                virtual void SomethingHappend(const Source event) = 0;
            };

            // Allow other processes to register/unregister from our notifications
            virtual uint32_t Register(ISamplePlugin::INotification* notification) = 0;
            virtual uint32_t Unregister(ISamplePlugin::INotification* notification) = 0;
        };
    }
}