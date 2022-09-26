#include "SamplePluginLink.h"

#include "../Log.h"

void SamplePluginLink::NotificationHandler::SomethingHappend(const Source event)
{
    Log("The \"Something happened\" notification fired");

    // This could perform custom logic based on the event. For example,
    // clients could register a callback to the various notifications raised by the
    // plugin
}

SamplePluginLink::SamplePluginLink(const string& callsign)
    : _callsign(callsign)
    , _samplePluginInterface(nullptr)
    , _samplePluginShell(nullptr)
    , _notificationHandler()
{
    Log("Constructing SamplePlugin link to callsign %s", _callsign.c_str());

    // Attempt to open the COM-RPC connection
    uint32_t success = BaseClass::Open(RPC::CommunicationTimeOut, BaseClass::Connector(), _callsign);

    if (success != Core::ERROR_NONE) {
        Log("Failed to open link to Thunder with error %s", Core::ErrorToString(success));
    } else {
        // For this example, show how to acquire another interface provided by the plugin. In this case,
        // acquire the shell so we can send activate/deactive commands from the client
        // TODO:: Is this safe to do here? What happens in non-happy-day scenarios with this interface?
        _samplePluginShell = BaseClass::Aquire<PluginHost::IShell>(RPC::CommunicationTimeOut, BaseClass::Connector(), _callsign);

        if (_samplePluginShell == nullptr) {
            Log("Failed to acquire IShell for sample plugin");
        }
    }
}

SamplePluginLink::~SamplePluginLink()
{
    BaseClass::Close(Core::infinite);

    if (_samplePluginInterface != nullptr) {
        _samplePluginInterface->Release();
        _samplePluginInterface = nullptr;
    }

    if (_samplePluginShell != nullptr) {
        _samplePluginShell->Release();
        _samplePluginShell = nullptr;
    }
}

/**
 * @brief Called when the state of the link changes so we can take appropriate action
 *
 * @param[in] upAndRunning  When true, the COM-RPC link is good and the plugin on the other end
 *                          is running and ready to receive requests. If false, the link is bad
 */
void SamplePluginLink::Operational(const bool upAndRunning)
{
    Log("Operational state has changed: %s", upAndRunning ? "Up" : "Down");

    if (upAndRunning) {
        // Connection is good - lets resolve our interfaces
        if (_samplePluginInterface == nullptr) {
            _samplePluginInterface = BaseClass::Interface();

            // Register our notification handler so it is called when the plugin
            // raises a notification
            _samplePluginInterface->Register(&_notificationHandler);
        }
    } else {
        // Connection is bad, unregister from notifications and release our interfaces
        if (_samplePluginInterface != nullptr) {
            // Always remember to unregister our notification handler too
            _samplePluginInterface->Unregister(&_notificationHandler);
            _samplePluginInterface->Release();
            _samplePluginInterface = nullptr;
        }
    }

    // If this was implemented as a library for use by clients, this could
    // trigger a callback to notify the client the plugin has changed operational
    // state
}

/**
 * @brief Return the operational state of the plugin
 *
 */
bool SamplePluginLink::IsOperational()
{
    return BaseClass::IsOperational();
}

/**
 * @brief Attempt to activate the plugin
 *
 * @return Thunder error code (ERROR_NONE for success)
 */
uint32_t SamplePluginLink::Activate()
{
    uint32_t errorCode = Core::ERROR_UNAVAILABLE;

    if (_samplePluginShell != nullptr) {
        errorCode = _samplePluginShell->Activate(PluginHost::IShell::REQUESTED);
    }

    return errorCode;
}

/**
 * @brief Attempt to deactivate the plugin
 *
 * @return Thunder error code (ERROR_NONE for success)
 */
uint32_t SamplePluginLink::Deactivate()
{
    uint32_t errorCode = Core::ERROR_UNAVAILABLE;

    if (_samplePluginShell != nullptr) {
        errorCode = _samplePluginShell->Deactivate(PluginHost::IShell::REQUESTED);
    }

    return errorCode;
}

/**
 * @brief Attempt to deactivate the plugin
 *
 * @return Thunder error code (ERROR_NONE for success)
 */
PluginHost::IShell::state SamplePluginLink::GetState()
{
    auto state = PluginHost::IShell::state::UNAVAILABLE;

    if (_samplePluginShell != nullptr) {
        state = _samplePluginShell->State();
    }

    return state;
}

// Begin methods from ISamplePlugin interface

uint32_t SamplePluginLink::Greet(const string& message, string& result)
{
    // If the link is down, return an unavailable error
    uint32_t errorCode = Core::ERROR_UNAVAILABLE;

    if (_samplePluginInterface != nullptr) {
        errorCode = _samplePluginInterface->Greet(message, result);
    }

    return errorCode;
}

uint32_t SamplePluginLink::Echo(const string& message, string& result)
{
    // If the link is down, return an unavailable error
    uint32_t errorCode = Core::ERROR_UNAVAILABLE;

    if (_samplePluginInterface != nullptr) {
        errorCode = _samplePluginInterface->Echo(message, result);
    }

    return errorCode;
}

// End methods from ISamplePlugin interface