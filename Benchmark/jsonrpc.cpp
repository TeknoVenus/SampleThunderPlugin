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

#include "jsonrpc.h"

#include <benchmark/benchmark.h>

#include "messages.h"

#include "Module.h"
#include <interfaces/ISamplePlugin.h>
#include <WPEFramework/interfaces/json/JsonData_SamplePlugin.h>

using namespace WPEFramework;
using namespace JsonData::SamplePlugin;

void JsonRpcFixture::SetUp(const ::benchmark::State &state)
{
    Core::SystemInfo::SetEnvironment(_T("THUNDER_ACCESS"), (_T("127.0.0.1:55555")));
    mRemoteObject = std::make_unique<JSONRPC::LinkType<Core::JSON::IElement>>("SamplePlugin.1", "");
}

void JsonRpcFixture::TearDown(const ::benchmark::State &state)
{
    //
}

void JsonRpcFixture::Echo100B(::benchmark::State &state)
{
    DoEcho(state, MessageStrings::OneHundredB);
}

void JsonRpcFixture::Echo1K(::benchmark::State &state)
{
    DoEcho(state, MessageStrings::OneKB);
}

void JsonRpcFixture::Echo10K(::benchmark::State &state)
{
    DoEcho(state, MessageStrings::TenKB);
}

void JsonRpcFixture::Echo100K(::benchmark::State &state)
{
    DoEcho(state, MessageStrings::OneHundredKB);
}

void JsonRpcFixture::Echo1M(::benchmark::State &state)
{
    DoEcho(state, MessageStrings::OneMB);
}

void JsonRpcFixture::DoEcho(::benchmark::State &state, const std::string &msg)
{
    EchoResultData result;
    EchoParamsData params;
    params.Data = msg;

    uint32_t success = mRemoteObject->Invoke<EchoParamsData, EchoResultData>(20000, "echo", params, result);

    if (success != Core::ERROR_NONE)
    {
        std::string errorMsg = "JSON-RPC returned error " + std::to_string(success) + " (" + std::string(Core::ErrorToString(success)) + ")";
        state.SkipWithError(errorMsg.c_str());
    }
}
