#pragma once

#include "Module.h"

// Include the interface we created
#include <interfaces/ISamplePlugin.h>
#include <WPEFramework/interfaces/json/JsonData_SamplePlugin.h>

namespace WPEFramework
{
    namespace Plugin
    {
        using namespace JsonData::SamplePlugin;

        /**
         * Sample plugin that exposes a COM-RPC and JSON-RPC API
         *
         * Exposes a COM-RPC and JSON-RPC API
         */
        class SamplePlugin : public PluginHost::IPlugin, public PluginHost::JSONRPC
        {
            class Notification : public RPC::IRemoteConnection::INotification,
                                 public Exchange::ISamplePlugin::INotification
            {
            private:
                Notification() = delete;
                Notification(const Notification &) = delete;
                Notification &operator=(const Notification &) = delete;

            public:
                explicit Notification(SamplePlugin *parent)
                    : _parent(*parent)
                {
                    ASSERT(parent != nullptr);
                }
                virtual ~Notification() override
                {
                }

            public:
                void SomethingHappend(const Source event) override
                {
                    _parent.SomethingHappend(event);
                }

                void Activated(RPC::IRemoteConnection * /* connection */) override
                {
                }

                // When we're deactivated, we want to do some of our own cleanup before we're fully destroyed,
                // so call our deactivated method
                void Deactivated(RPC::IRemoteConnection *connection) override
                {
                    _parent.Deactivated(connection);
                }

                // Build QueryInterface implementation, specifying all possible interfaces to be returned.
                BEGIN_INTERFACE_MAP(Notification)
                INTERFACE_ENTRY(Exchange::ISamplePlugin::INotification)
                INTERFACE_ENTRY(RPC::IRemoteConnection::INotification)
                END_INTERFACE_MAP

            private:
                SamplePlugin &_parent;
            };

        public:
            SamplePlugin();
            ~SamplePlugin() override;

            // Do not allow copy/move constructors
            SamplePlugin(const SamplePlugin &) = delete;
            SamplePlugin &operator=(const SamplePlugin &) = delete;

            // Build QueryInterface implementation, specifying all possible interfaces to be returned.
            BEGIN_INTERFACE_MAP(SamplePlugin)
            INTERFACE_ENTRY(PluginHost::IPlugin)
            INTERFACE_ENTRY(PluginHost::IDispatcher)
            END_INTERFACE_MAP

        public:
            // Implement the basic IPlugin interface that all plugins must implement
            const string Initialize(PluginHost::IShell *service) override;
            void Deinitialize(PluginHost::IShell *service) override;
            string Information() const override;

        private:
            // Notification/event handlers
            // Clean up when we're told to deactivate
            void Deactivated(RPC::IRemoteConnection *connection);

            // Our custom notification
            void SomethingHappend(const Exchange::ISamplePlugin::INotification::Source event);

        private:
            // JSON-RPC setup
            void RegisterAllMethods();
            void UnregisterAllMethods();

            uint32_t Greeter(const GreeterParamsData& params, GreeterResultData& response);

        private:
            uint32_t _connectionId;
            PluginHost::IShell *_service;
            Exchange::ISamplePlugin *_samplePlugin;
            Core::Sink<Notification> _notification;
        };
    }
}