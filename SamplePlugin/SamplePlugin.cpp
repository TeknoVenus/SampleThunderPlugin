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
              _notification(this),
              _notificationCallbacks({}),
              _greetings({"Hello", "Goodbye", "Hi", "Bye", "Good Morning", "Good Afternoon"}) // The different greeting messages we can generate
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
            _samplePlugin = service->Root<Exchange::ISamplePlugin>(_connectionId, 2000, _T("SamplePlugin"));

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
            // TODO:: Understand exactly what this is for
            if (connection->Id() == _connectionId)
            {
                ASSERT(_service != nullptr);
                Core::IWorkerPool::Instance().Submit(PluginHost::IShell::Job::Create(_service, PluginHost::IShell::DEACTIVATED, PluginHost::IShell::FAILURE));
            }
        }

        /**
         * Our notification handler
         *
         * TODO:: Fully understand notifications
         */
        void SamplePlugin::SomethingHappend(const Exchange::ISamplePlugin::INotification::Source event)
        {
            TRACE(Trace::Information, (_T("Raising a notification")));
        }

        /**
         * The actual method we want to implement from the interface
         *
         * Generate a greeting
         *
         * @param[in] message   Who the greeting is for
         * @param[out] result   The generated greeting
         */
        uint32_t SamplePlugin::Greet(const string &message, string &result /* @out */)
        {
            TRACE(Trace::Information, (_T("Generating greeting")));
            TRACE(Trace::Information, (_T("Running in process %d"), Core::ProcessInfo().Id()));

            // Pick a random greeting from the pre-determined list
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> distribution(0, _greetings.size() - 1);

            std::string greeting = _greetings[distribution(rng)];

            // Trigger a notification for anyone who's listening
            // TODO:: Understand notifications better
            std::lock_guard<std::mutex> locker(_notificationMutex);
            for (const auto callback : _notificationCallbacks)
            {
                callback->SomethingHappend(ISamplePlugin::INotification::EXCITING_THING_HAPPENED);
            }

            // Build the final message
            result = greeting + ", " + message;

            // All good - return success
            return Core::ERROR_NONE;
        }

        /**
         * Register a notification callback
         */
        uint32_t SamplePlugin::Register(ISamplePlugin::INotification *notification)
        {
            std::lock_guard<std::mutex> locker(_notificationMutex);

            // Make sure we can't register the same notification callback multiple times
            if (std::find(_notificationCallbacks.begin(), _notificationCallbacks.end(), notification) == _notificationCallbacks.end())
            {
                _notificationCallbacks.push_back(notification);
                notification->AddRef();
            }

            return Core::ERROR_NONE;
        }

        /**
         * Unregister a notification callback
         */
        uint32_t SamplePlugin::Unregister(ISamplePlugin::INotification *notification)
        {
            std::lock_guard<std::mutex> locker(_notificationMutex);

            // Make sure we can't register the same notification callback multiple times
            auto itr = std::find(_notificationCallbacks.begin(), _notificationCallbacks.end(), notification);
            if (itr != _notificationCallbacks.end())
            {
                (*itr)->Release();
                _notificationCallbacks.erase(itr);
            }

            return Core::ERROR_NONE;
        }
    }
}
