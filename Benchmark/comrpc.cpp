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

#include "comrpc.h"

#include <benchmark/benchmark.h>

#include "Module.h"
#include <interfaces/ISamplePlugin.h>

#include "messages.h"

using namespace WPEFramework;

void ComRpcFixture::SetUp(const ::benchmark::State &state)
{
    mEngine = Core::ProxyType<RPC::InvokeServerType<1, 0, 4>>::Create();
    mClient = Core::ProxyType<RPC::CommunicatorClient>::Create(Core::NodeId("/tmp/communicator"), Core::ProxyType<Core::IIPCServer>(mEngine));

    mEngine->Announcements(mClient->Announcement());

    if (!mClient.IsValid())
    {
        fprintf(stderr, "Invalid client\n");
    }

    mController = mClient->Open<PluginHost::IShell>(_T("SamplePlugin"), ~0, 3000);
}

void ComRpcFixture::TearDown(const ::benchmark::State &state)
{
    if (mController)
    {
        mController->Release();
    }

    // Disconnect from the COM-RPC socket
    mClient->Close(RPC::CommunicationTimeOut);
    if (mClient.IsValid())
    {
        mClient.Release();
    }
}

void ComRpcFixture::Echo100B(::benchmark::State &state)
{
    DoEcho(state, MessageStrings::OneHundredB);
}

void ComRpcFixture::Echo1K(::benchmark::State &state)
{
    DoEcho(state, MessageStrings::OneKB);
}

void ComRpcFixture::Echo10K(::benchmark::State &state)
{
    DoEcho(state, MessageStrings::TenKB);
}

void ComRpcFixture::Echo100K(::benchmark::State &state)
{
    DoEcho(state, MessageStrings::OneHundredKB);
}

void ComRpcFixture::Echo1M(::benchmark::State &state)
{
    DoEcho(state, MessageStrings::OneMB);
}


void ComRpcFixture::DoEcho(::benchmark::State &state, const std::string& msg)
{
    if (mController)
    {
        auto samplePlugin = mController->QueryInterface<Exchange::ISamplePlugin>();
        samplePlugin->AddRef();

        if (samplePlugin)
        {
            std::string result;

            auto success = samplePlugin->Echo(msg, result);
            samplePlugin->Release();

            if (success != Core::ERROR_NONE)
            {
                std::string errorMsg = "COM-RPC returned error " + std::to_string(success) + " (" + std::string(Core::ErrorToString(success)) + ")";
                state.SkipWithError(errorMsg.c_str());
            }

        }
    }
    else
    {
        state.SkipWithError("Failed to create controller");
    }
}