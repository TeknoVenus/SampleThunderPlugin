#include "SamplePluginImplementation.h"

namespace WPEFramework
{
    namespace Plugin
    {
        SERVICE_REGISTRATION(SamplePluginImplementation, 1, 0);

        SamplePluginImplementation::SamplePluginImplementation()
            : _notificationCallbacks({})
        {
        }

        /**
         * The actual method we want to implement from the interface
         */
        uint32_t SamplePluginImplementation::Greet(const string &message, string &result /* @out */) const
        {
            printf("Doing greet implementation!\n");

            result = "Hello, " + message;

            return Core::ERROR_NONE;
        }

        /**
         * Register a notification callback
         */
        uint32_t SamplePluginImplementation::Register(ISamplePlugin::INotification* notification)
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
        uint32_t SamplePluginImplementation::Unregister(ISamplePlugin::INotification* notification)
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