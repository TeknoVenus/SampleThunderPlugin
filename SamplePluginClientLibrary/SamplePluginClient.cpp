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


#include "SamplePluginClient.h"
#include <stdio.h>
#include <chrono>

#include <interfaces/IMemory.h>
#include <interfaces/IDictionary.h>

// Start notification handlers

/**
 * Callback that should be fired when the plugin emits this notification
 *
 * More complex plugins will likely have lots of different notifications we can
 * register distinct callbacks for
 */
void SamplePluginClient::NotificationHandler::SomethingHappend(const Source event)
{
    // Just print something for now
    if (event == EXCITING_THING_HAPPENED)
    {
        Log("Received an exciting notification!");
    }
    else if (event == BORING_THING_HAPPENED)
    {
        Log("Received a boring notification!");
    }
}

// End notification handlers

SamplePluginClient::SamplePluginClient()
    : mRemoteConnection(GetConnectionEndpoint()),
      mEngine(Core::ProxyType<RPC::InvokeServerType<1, 0, 4>>::Create()),
      mClient(Core::ProxyType<RPC::CommunicatorClient>::Create(mRemoteConnection, Core::ProxyType<Core::IIPCServer>(mEngine))),
      mValid(false),
      mNotification()
{
    // Announce our arrival over COM-RPC
    mEngine->Announcements(mClient->Announcement());

    // Check we opened the link correctly (if Thunder isn't running, this will be false)
    if (!mClient.IsValid())
    {
        Log("Failed to open link");
        mValid = false;
        return;
    }

    Log("Connecting to Thunder @ '%s'", mClient->Source().RemoteId().c_str());

    // Open a connection to Thunder
    /* Notes: There are 2 ways to connect to a plugin over COM-RPC:

    1) Plugin creates its own COM-RPC server and we directly connect to the plugin. The plugin will
    advertise its own interfaces over this COM-RPC server and we can open a connection to directly get their interface(s).

    This is how OCDM works

    2) Thunder exposes a generic /tmp/communicator COM-RPC server, which is designed mainly for out-of-process
    registration and communication. However, it also allows us to get the IShell interface of any plugin, which
    means we can control the lifecycle of the plugin. We can then traverse this Shell to work out what other interfaces
    this plugin implements.

    This is not ideal as if the plugin has a separate implementation that registers the COM-RPC interface but not IShell
    we can't traverse our way to it from /tmp/communicator.

    High-bandwidth/latency sensitive plugins` should also use their own sockets so they can configure appropriate buffer sizes and prevent
    the connection from being backed up with other requests. However this was beyond the scope of this example.
    */

    mController = mClient->Open<PluginHost::IShell>(_T("SamplePlugin"), ~0, 3000);
    if (!mController)
    {
        Log("Failed to open IShell interface of SamplePlugin - is Thunder running?");
        mValid = false;
        return;
    }

    // Plugin must be activated to query the interface, so activate it now
    if (!ActivateSamplePlugin())
    {
        mValid = false;
        return;
    }

    // Now traverse the shell and get the interface we actually care about
    // If the plugin implemented many interfaces, query them one at a time
    mSamplePlugin = mController->QueryInterface<Exchange::ISamplePlugin>();
    if (!mSamplePlugin)
    {
        Log("Failed to open ISamplePlugin interface of SamplePlugin - is Thunder running?");
        mValid = false;
        return;
    }

    // Register for notifications - we want our callbacks to fire when the plugin emits a notification
    mSamplePlugin->AddRef();
    mSamplePlugin->Register(&mNotification);

    // All good
    mValid = true;
}

SamplePluginClient::~SamplePluginClient()
{
    // Clean up
    if (mController)
    {
        mController->Release();
    }

    if (mSamplePlugin)
    {
        // Remove our notification callback
        mSamplePlugin->Unregister(&mNotification);

        // Clean up
        mSamplePlugin->Release();
    }

    // Disconnect from the COM-RPC socket
    mClient->Close(RPC::CommunicationTimeOut);
    if (mClient.IsValid() == true)
    {
        mClient.Release();
    }

    // Dispose of any singletons we created (Thunder uses a lot of singletons internally)
    Core::Singleton::Dispose();
}

/**
 * Attempt to activate the plugin 'SamplePlugin' and prints how long it took
 *
 * @return true if activated successfully
 */
bool SamplePluginClient::ActivateSamplePlugin()
{
    if (mController->State() == PluginHost::IShell::ACTIVATED)
    {
        // Already activated, nothing to do
        return true;
    }

    Log("SamplePlugin is deactivated - activating now");

    // Start a stopwatch so we can track how long this takes
    auto start = std::chrono::high_resolution_clock::now();
    uint32_t result = mController->Activate(PluginHost::IShell::REQUESTED);

    if (result != Core::ERROR_NONE)
    {
        Log("Failed to activate SamplePlugin with error %d (%s)", result, Core::ErrorToString(result));
        return false;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto activationTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    Log("Activated SamplePlugin plugin successfully in %lu ms", activationTime.count());
    return true;
}

/**
 * Attempt to deactivate the plugin 'SamplePlugin' and prints how long it took
 *
 * @return true if deactivated successfully
 */
bool SamplePluginClient::DeactivateSamplePlugin()
{
    if (mController->State() == PluginHost::IShell::DEACTIVATED)
    {
        // Already deactivated, nothing to do
        return true;
    }

    Log("Deactivating plugin");

    // Start a stopwatch so we can track how long this takes
    auto start = std::chrono::high_resolution_clock::now();
    uint32_t result = mController->Deactivate(PluginHost::IShell::REQUESTED);

    if (result != Core::ERROR_NONE)
    {
        Log("Failed to deactivate SamplePlugin with error %d (%s)", result, Core::ErrorToString(result));
        return false;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto deactivationTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    Log("Deactivated plugin successfully in %lu ms", deactivationTime.count());
    return true;
}

/**
 * Return true if we connected to Thunder successfully and managed to
 * find the COM-RPC interface(s) we care about
 */
bool SamplePluginClient::IsValid()
{
    return mValid;
}

/**
 * Actually call out over COM-RPC to invoke the plugin method we implemented
 */
std::string SamplePluginClient::GetGreeting(const std::string &message)
{
    if (mSamplePlugin)
    {
        std::string greeting;
        uint32_t result = mSamplePlugin->Greet(message, greeting);
        if (result == Core::ERROR_NONE)
        {
            return greeting;
        }
        else
        {
            // Something went wrong
            Log("Failed to generate greeting with error %d (%s)", result, Core::ErrorToString(result));
            return "";
        }
    }

    Log("Cannot generate greeting - not connected to SamplePlugin");
    return "";
}

/**
 * Retrieves the socket we will communicate over for COM-RPC
 */
Core::NodeId SamplePluginClient::GetConnectionEndpoint()
{
    std::string communicatorPath;
    Core::SystemInfo::GetEnvironment(_T("COMMUNICATOR_PATH"), communicatorPath);

    // On linux, Thunder defaults to /tmp/communicator for the generic COM-RPC
    // interface
    if (communicatorPath.empty())
    {
        communicatorPath = "/tmp/communicator";
    }

    return Core::NodeId(communicatorPath.c_str());
}