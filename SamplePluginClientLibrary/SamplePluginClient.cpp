#include "SamplePluginClient.h"
#include <stdio.h>
#include <chrono>

#include <interfaces/IMemory.h>
#include <interfaces/IDictionary.h>

SamplePluginClient::SamplePluginClient()
    : mRemoteConnection(GetConnectionEndpoint()),
      mEngine(Core::ProxyType<RPC::InvokeServerType<1, 0, 4>>::Create()),
      mClient(Core::ProxyType<RPC::CommunicatorClient>::Create(mRemoteConnection, Core::ProxyType<Core::IIPCServer>(mEngine))),
      mValid(false)
{
    mEngine->Announcements(mClient->Announcement());

    if (!mClient.IsValid())
    {
        Log("Failed to open link");
        return;
    }

    Log("Connected to Thunder @ '%s'", mClient->Source().RemoteId().c_str());

    mController = mClient->Open<PluginHost::IShell>(_T("SamplePlugin"), ~0, 3000);
    if (!mController)
    {
        Log("Failed to connect to Thunder Controller");
        return;
    }

    // Activate the plugin if it's not already
    if (mController->State() == PluginHost::IShell::DEACTIVATED)
    {
        Log("SamplePlugin is deactivated - activating now");

        auto start = std::chrono::high_resolution_clock::now();
        uint32_t result = mController->Activate(PluginHost::IShell::REQUESTED);

        if (result == Core::ERROR_NONE)
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto activationTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            Log("Activated plugin successfully in %lu ms", activationTime.count());
        }
        else
        {
            Log("Failed to activate SamplePlugin with error %d (%s)", result, Core::ErrorToString(result));
            return;
        }
    }

    mSamplePlugin = mController->QueryInterface<Exchange::ISamplePlugin>();
    if (!mSamplePlugin)
    {
        Log("Could not find ISamplePlugin interface - does the plugin actually implement it?");
        return;
    }

    mValid = true;
}

SamplePluginClient::~SamplePluginClient()
{
    if (mValid)
    {
        Log("Deactivating plugin");

        auto start = std::chrono::high_resolution_clock::now();
        uint32_t result = mController->Deactivate(PluginHost::IShell::REQUESTED);

        if (result == Core::ERROR_NONE)
        {
            auto end = std::chrono::high_resolution_clock::now();
            auto deactivationTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            Log("Deactivated plugin successfully in %lu ms", deactivationTime.count());
        }
        else
        {
            Log("Failed to deactivate SamplePlugin with error %d (%s)", result, Core::ErrorToString(result));
            return;
        }

        mSamplePlugin->Release();
        mController->Release();
    }

    mClient->Close(RPC::CommunicationTimeOut);
    if (mClient.IsValid() == true)
    {
        mClient.Release();
    }
    Core::Singleton::Dispose();
}

std::string SamplePluginClient::GetGreeting(const std::string &message)
{
    if (mSamplePlugin)
    {
        std::string greeting;

        uint32_t result = mSamplePlugin->Greet("Stephen", greeting);
        if (result == Core::ERROR_NONE)
        {
            return greeting;
        }
        else
        {
            Log("Failed to generate greeting with error %d (%s)", result, Core::ErrorToString(result));
            return "";
        }
    }

    Log("Cannot generate greeting - not connected to SamplePlugin");
    return "";
}

/**
 * Retrieves the unix socket we will communicate over for COM-RPC
 */
Core::NodeId SamplePluginClient::GetConnectionEndpoint()
{
    std::string communicatorPath;
    Core::SystemInfo::GetEnvironment(_T("COMMUNICATOR_PATH"), communicatorPath);

    if (communicatorPath.empty())
    {
        communicatorPath = "/tmp/communicator";
    }

    return Core::NodeId(communicatorPath.c_str());
}