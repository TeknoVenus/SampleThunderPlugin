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

#include "SamplePlugin.h"

#include <random>

namespace WPEFramework
{
    namespace Plugin
    {
        const short SamplePlugin::API_VERSION_NUMBER_MAJOR = 1;
        const short SamplePlugin::API_VERSION_NUMBER_MINOR = 0;

        SERVICE_REGISTRATION(SamplePlugin, SamplePlugin::API_VERSION_NUMBER_MAJOR, SamplePlugin::API_VERSION_NUMBER_MINOR);

        SamplePlugin::SamplePlugin()
            : _connectionId(0),
              _service(nullptr),
              _samplePlugin(nullptr),
              _notification(this)
        {
            // Don't do any work in the constructor - all set up should be done in Initialize
        }

        SamplePlugin::~SamplePlugin()
        {
            // Don't do any work in the constructor - all tear down should be done in Deinitialize
        }

        /**
         * Initialise the plugin and register ourselves
         *
         * This should aim to be as fast as possible
         *
         * Even if we're running in Out Of Process mode, this will still run in the
         * main WPEFramework process - the new process is actually spawned from this method
         */
        const string SamplePlugin::Initialize(PluginHost::IShell *service)
        {
            // Make sure everything is null as we expect
            ASSERT(_service == nullptr);
            ASSERT(_samplePlugin == nullptr);

            // Syslog Startup messages are always printed by default
            SYSLOG(Logging::Startup, (_T("Initializing SamplePlugin")));
            SYSLOG(Logging::Startup, (_T("Initialize running in process %d"), Core::ProcessInfo().Id()));

            // Register the Connection::Notification first. Do this before we start our actual plugin
            // in case something goes wrong or is disconnected - this way we know we're at least registered
            // for activate/deactivate events
            _service = service;
            _service->Register(&_notification);

            // Register ourselves in the PluginHost so other plugins know where to find us
            // If we are running out of process (as per our config file), this is what will actually spawn the WPEProcess process
            // which will run our plugin instance
            //
            // Ideally for large, complex plugins we would actually split the plugin into two libraries - a thin library that just calls
            // _service->Root to launch WPEProcess, and a larger library that is only ever run inside WPEProcess only (we do this for Cobalt and WebKitBrowser)
            _samplePlugin = service->Root<Exchange::ISamplePlugin>(_connectionId, 2000, _T("SamplePluginImplementation"));

            // Still running inside the main WPEFramework process - the child process will have now been spawned and registered if necessary
            if (_samplePlugin != nullptr)
            {
                _samplePlugin->Register(&_notification);

                // Register all our JSON-RPC methods
                RegisterAllMethods();
            }
            else
            {
                // Something went wrong, clean up
                TRACE(Trace::Error, (_T("Failed to initialize SamplePlugin")));
                _service->Unregister(&_notification);
                _service = nullptr;

                // Returning a string signals that we failed to initialize - WPEFramework will print this as an error message
                return "Failed to initialize SamplePlugin";
            }

            // Success
            return "";
        }

        /**
         * Clean up the plugin when we're deactivated. Should release any resources we were holding
         *
         * Note again this code runs inside the main WPEFramework daemon even if the plugin is set to run out-of-process
         */
        void SamplePlugin::Deinitialize(PluginHost::IShell *service)
        {
            ASSERT(_service == service);
            ASSERT(_samplePlugin != nullptr);

            TRACE(Trace::Information, (_T("Deinitializing SamplePlugin")));
            TRACE(Trace::Information, (_T("Deinitialize running in process %d"), Core::ProcessInfo().Id()));

            if (_samplePlugin != nullptr)
            {
                // TODO:: Work out exactly what triggers the shutdown of the out-of-process host
                _service->Unregister(&_notification);
                _samplePlugin->Unregister(&_notification);

                // Unregister all our JSON-RPC methods
                UnregisterAllMethods();
                _samplePlugin->Release();
            }

            // Set everything back to default
            _connectionId = 0;
            _service = nullptr;
            _samplePlugin = nullptr;
        }

        string SamplePlugin::Information() const
        {
            // No additional info to report
            return string();
        }


        void SamplePlugin::Deactivated(RPC::IRemoteConnection *connection)
        {
            // Gracefully handle an unexpected termination from the other side of the connection (for example if the remote process crashed)
            // and deactivate ourselves as we cannot continue safely
            if (connection->Id() == _connectionId)
            {
                ASSERT(_service != nullptr);
                Core::IWorkerPool::Instance().Submit(PluginHost::IShell::Job::Create(_service, PluginHost::IShell::DEACTIVATED, PluginHost::IShell::FAILURE));
            }
        }

        /**
         * Our notification handler
         *
         * This will run inside the main WPEFramework process, but will be called from the COM-RPC side
         * of the plugin
         *
         * Use this to raise a JSONRPC notification
         */
        void SamplePlugin::SomethingHappend(const Exchange::ISamplePlugin::INotification::Source event)
        {
            // A notification occurred on the COM-RPC side of the plugin
            TRACE(Trace::Information, (_T("Handled a notification in process %d"), Core::ProcessInfo().Id()));
        }
    }
}
