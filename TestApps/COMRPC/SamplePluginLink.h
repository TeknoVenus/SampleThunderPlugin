#pragma once

#include "Module.h"
#include <interfaces/ISamplePlugin.h>

using namespace WPEFramework;
using BaseClass = RPC::SmartInterfaceType<Exchange::ISamplePlugin>;

/**
 * @brief Connection to the Sample Plugin over COM-RPC
 *
 * This is a very minimal implementation, but this could easily be extended to
 * (for example) allow clients to register callbacks so they can run their own
 * code when a notification occurs, or when the plugin/com-rpc link changes
 * operational state (e.g. if the plugin is deactivated)
 *
 * Uses the SmartLink which can automatically handle disconnections and plugin
 * failures for us!
 */
class SamplePluginLink : protected BaseClass {

    /**
     * To handle notifications, we must create a concrete implementation of the
     * plugin's notification interface on the client side. When we connect to the
     * plugin, we register this class with the plugin. Then, when the plugin raises
     * a notification on the server, it calls all registered implementations of INotification
     * and we run our client-side code
     */
    class NotificationHandler : public Exchange::ISamplePlugin::INotification {
        void SomethingHappend(const Source event) override;

        // Must define an interface map since we are implementing an interface on the exchange
        // so Thunder knows what type we are
        BEGIN_INTERFACE_MAP(NotificationHandler)
        INTERFACE_ENTRY(Exchange::ISamplePlugin::INotification)
        END_INTERFACE_MAP
    };

public:
    // Upon constucting this object, we will attempt to connect to Thunder
    SamplePluginLink(const string& callsign);

    // Destructing this link will disconnect us from Thunder
    ~SamplePluginLink() override;

    SamplePluginLink(const SamplePluginLink&) = delete;
    SamplePluginLink& operator=(const SamplePluginLink&) = delete;

    // Run our own code when the operational status of the plugin changes
    void Operational(const bool upAndRunning) override;

public:
    bool IsOperational();

    // These are convenience wrappers over the methods from the interface
    uint32_t Greet(const string& message, string& result);
    uint32_t Echo(const string& message, string& result);

    // These are convience wrappers over the plugin shell
    // Be careful exposing these to clients, you probably don't want client applications to be able to
    // control your plugin lifecycle!
    uint32_t Activate();
    uint32_t Deactivate();
    PluginHost::IShell::state GetState();

private:
    const string _callsign;
    Exchange::ISamplePlugin* _samplePluginInterface;

    // Only bother acquiring this interface if you need to be able to control plugin
    // lifecycle or find out metadata about the plugin
    PluginHost::IShell* _samplePluginShell;

    Core::Sink<NotificationHandler> _notificationHandler;
};