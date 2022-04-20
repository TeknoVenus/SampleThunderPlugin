#include "SamplePlugin.h"

namespace WPEFramework
{
    namespace Plugin
    {
        SERVICE_REGISTRATION(SamplePlugin, 1, 0);

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
         */
        const string SamplePlugin::Initialize(PluginHost::IShell *service)
        {
            // Make sure everything is null as we expect
            ASSERT(_service == nullptr);
            ASSERT(_samplePlugin == nullptr);

            // Register the Connection::Notification first. Do this before we start our actual plugin
            // in case something goes wrong or is disconnected - this way we know we're at least registered
            // for activate/deactivate events
            _service = service;
            _service->Register(&_notification);

            _samplePlugin = service->Root<Exchange::ISamplePlugin>(_connectionId, 2000, _T("SamplePluginImplementation"));

            if (_samplePlugin != nullptr)
            {
                _samplePlugin->Register(&_notification);

                // Register all our JSON-RPC methods as well
                RegisterAllMethods();
            }
            else
            {
                // Something went wrong, clean up
                _service->Unregister(&_notification);
                _service = nullptr;
                return "Failed to instantiate SamplePlugin";
            }

            // Success
            return "";
        }

        void SamplePlugin::Deinitialize(PluginHost::IShell *service)
        {
            ASSERT(_service == service);
            ASSERT(_samplePlugin != nullptr);

            if (_samplePlugin != nullptr)
            {
                _service->Unregister(&_notification);
                _samplePlugin->Unregister(&_notification);

                // Unregister JSON-RPC methods
                UnregisterAllMethods();
                _samplePlugin->Release();
            }

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
            // TODO:: Understand why we do this...
            if (connection->Id() == _connectionId)
            {
                ASSERT(_service != nullptr);
                Core::IWorkerPool::Instance().Submit(PluginHost::IShell::Job::Create(_service, PluginHost::IShell::DEACTIVATED, PluginHost::IShell::FAILURE));
            }
        }
    }
}
