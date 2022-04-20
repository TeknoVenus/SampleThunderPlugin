#pragma once

#include "SamplePlugin.h"

#include <mutex>
#include <list>

namespace WPEFramework
{
    namespace Plugin
    {
        class SamplePluginImplementation : public Exchange::ISamplePlugin
        {
        public:
            SamplePluginImplementation();
            SamplePluginImplementation(const SamplePluginImplementation &) = delete;
            SamplePluginImplementation &operator=(const SamplePluginImplementation &) = delete;

            virtual ~SamplePluginImplementation()
            {
            }

            BEGIN_INTERFACE_MAP(SamplePluginImplementation)
            INTERFACE_ENTRY(Exchange::ISamplePlugin)
            END_INTERFACE_MAP

        public:
            uint32_t Greet(const string &message, string &result /* @out */) const override;

        private:
            uint32_t Register(ISamplePlugin::INotification* notification) override;
            uint32_t Unregister(ISamplePlugin::INotification* notification) override;

        private:
            std::list<Exchange::ISamplePlugin::INotification *> _notificationCallbacks;
            std::mutex _notificationMutex;
        };
    }
}