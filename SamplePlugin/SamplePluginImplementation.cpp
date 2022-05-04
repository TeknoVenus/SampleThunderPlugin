#include "SamplePluginImplementation.h"

#include <random>

namespace WPEFramework
{
    namespace Plugin
    {
        SERVICE_REGISTRATION(SamplePluginImplementation, 1, 0);

        SamplePluginImplementation::SamplePluginImplementation()
            : _notificationCallbacks({}),
              _greetings({"Hello", "Goodbye", "Hi", "Bye", "Good Morning", "Good Afternoon"}) // The different greeting messages we can generate
        {
        }

        SamplePluginImplementation::~SamplePluginImplementation()
        {
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
        uint32_t SamplePluginImplementation::Greet(const string &message, string &result /* @out */)
        {
            TRACE(Trace::Information, (_T("Generating greeting")));
            TRACE(Trace::Information, (_T("Running in process %d"), Core::ProcessInfo().Id()));

            // Pick a random greeting from the pre-determined list
            std::random_device dev;
            std::mt19937 rng(dev());
            std::uniform_int_distribution<std::mt19937::result_type> distribution(0, _greetings.size() - 1);

            std::string greeting = _greetings[distribution(rng)];

            // Build the final message
            result = greeting + ", " + message;

            // Send out a notification that we generated a greeting
            // Loop through all the registered callbacks and fire off the notification
            std::lock_guard<std::mutex> locker(_notificationMutex);
            TRACE(Trace::Information, (_T("We have %d callbacks to trigger"), _notificationCallbacks.size()));
            for (const auto callback : _notificationCallbacks)
            {
                callback->SomethingHappend(ISamplePlugin::INotification::EXCITING_THING_HAPPENED);
            }

            // All good - return success
            return Core::ERROR_NONE;
        }

        /**
         * Register a notification callback
         */
        uint32_t SamplePluginImplementation::Register(ISamplePlugin::INotification *notification)
        {
            std::lock_guard<std::mutex> locker(_notificationMutex);

            // Make sure we can't register the same notification callback multiple times
            if (std::find(_notificationCallbacks.begin(), _notificationCallbacks.end(), notification) == _notificationCallbacks.end())
            {
                SYSLOG(Logging::Startup, (_T("Added a callback (Running in process %d)"), Core::ProcessInfo().Id()));
                _notificationCallbacks.push_back(notification);
                notification->AddRef();
            }

            return Core::ERROR_NONE;
        }

        /**
         * Unregister a notification callback
         */
        uint32_t SamplePluginImplementation::Unregister(ISamplePlugin::INotification *notification)
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