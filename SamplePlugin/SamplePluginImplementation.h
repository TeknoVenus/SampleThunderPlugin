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