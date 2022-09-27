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

#include "SamplePluginImplementation.h"

namespace WPEFramework {
namespace Plugin {
    // Don't use metadata here, use the normal service registration
    SERVICE_REGISTRATION(SamplePluginImplementation, 1, 0);

    SamplePluginImplementation::SamplePluginImplementation()
        : _notificationCallbacks({})
        , _greetings({})
        , _rng(_randomDevice())
    {
    }

    SamplePluginImplementation::~SamplePluginImplementation()
    {
    }

    /**
     * We allow configuring this plugin from the config file
     *
     * Parse the JSON config and act upon it
     */
    uint32_t SamplePluginImplementation::Configure(PluginHost::IShell* framework)
    {
        _config.FromString(framework->ConfigLine());

        // Add all the greetings in the config to our list of greetings
        if (_config.Greetings.IsSet() && !_config.Greetings.IsNull()) {
            auto it = _config.Greetings.Elements();
            while (it.Next()) {
                _greetings.emplace_back(it.Current().Value());
            }
        } else {
            TRACE(Trace::Warning, (_T("No greetings configured")));
        }

        return Core::ERROR_NONE;
    }

    /**
     * The actual method we want to implement from the interface
     *
     * If running out of process, this will run in the out-of-process part and return
     * the result over COM-RPC
     *
     * Generate a greeting
     *
     * @param[in] message   Who the greeting is for
     * @param[out] result   The generated greeting
     */
    uint32_t SamplePluginImplementation::Greet(const string& message, string& result)
    {
        uint32_t success = Core::ERROR_NONE;

        if (message.empty()) {
            TRACE(Trace::Warning, (_T("Message is required")));
            success = Core::ERROR_BAD_REQUEST;
        } else {
            if (_greetings.size() != 0) {
                TRACE(Trace::Information, (_T("Generating greeting in process %d"), Core::ProcessInfo().Id()));
                // Pick a random greeting from the ones in our config file
                std::uniform_int_distribution<std::mt19937::result_type> distribution(0, _greetings.size() - 1);

                string greeting = _greetings[distribution(_rng)];

                // Build the final message
                result = greeting + ", " + message;

                // Send out a notification that we generated a greeting
                RaiseSomethingHappenedNotification(INotification::Source::EXCITING_THING_HAPPENED);
            } else {
                TRACE(Trace::Fatal, (_T("No greetings configured - cannot generate greeting!")));
                success = Core::ERROR_INCOMPLETE_CONFIG;
            }
        }

        return success;
    }

    /**
     * A simple echo method used for benchmarking. Returns the data it was given
     * without modification
     *
     * @param[in] message   Message to be echo'd
     * @param[out] result   Echo'd data
     */
    uint32_t SamplePluginImplementation::Echo(const string& message, string& result)
    {
        // Just return exactly what we were sent - even if it's empty
        result = message;

        // All good - return success
        return Core::ERROR_NONE;
    }

    /**
     * Register a notification callback
     */
    uint32_t SamplePluginImplementation::Register(ISamplePlugin::INotification* notification)
    {
        _notificationMutex.Lock();

        // Make sure we can't register the same notification callback multiple times
        if (std::find(_notificationCallbacks.begin(), _notificationCallbacks.end(), notification) == _notificationCallbacks.end()) {
            _notificationCallbacks.push_back(notification);
            notification->AddRef();
        }

        _notificationMutex.Unlock();

        return Core::ERROR_NONE;
    }

    /**
     * Unregister a notification callback
     */
    uint32_t SamplePluginImplementation::Unregister(ISamplePlugin::INotification* notification)
    {
        _notificationMutex.Lock();

        // Make sure we can't register the same notification callback multiple times
        auto itr = std::find(_notificationCallbacks.begin(), _notificationCallbacks.end(), notification);
        if (itr != _notificationCallbacks.end()) {
            (*itr)->Release();
            _notificationCallbacks.erase(itr);
        }

        _notificationMutex.Unlock();

        return Core::ERROR_NONE;
    }

    void SamplePluginImplementation::RaiseSomethingHappenedNotification(const INotification::Source& eventSource)
    {
        // Loop through all the registered callbacks and fire off the notification

        // If we wanted to be even more robust, we should observe the Revoked event from ICOMLink::INotification
        // which is triggered when a client crashes and remove dead callbacks. See the COMRPCPluginServer example
        // in ThunderNanoServices

        _notificationMutex.Lock();

        TRACE(Trace::Information, (_T("We have %d subscribed clients to trigger notifications for"), _notificationCallbacks.size()));
        for (const auto callback : _notificationCallbacks) {
            callback->SomethingHappend(ISamplePlugin::INotification::EXCITING_THING_HAPPENED);
        }

        _notificationMutex.Unlock();
    }
}
}