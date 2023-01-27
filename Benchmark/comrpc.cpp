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

ComRpcFixture::ComRpcFixture()
    : _samplePluginSmartLink()
    , _linkOpen(false)
{
    Iterations(20);
}

ComRpcFixture::~ComRpcFixture()
{
    printf("dtor\n");
}

void ComRpcFixture::SetUp(::benchmark::State& state)
{
    auto success = _samplePluginSmartLink.Open(RPC::CommunicationTimeOut, _samplePluginSmartLink.Connector(), "SamplePlugin");

    if (success != Core::ERROR_NONE) {
        string error = "Failed to establish connection with Thunder with error " + string(Core::ErrorToString(success));
        state.SkipWithError(error.c_str());
    } else {
        _linkOpen = true;
    }
}

void ComRpcFixture::TearDown(const ::benchmark::State& state)
{
    if (_linkOpen) {
        printf("teardown\n");
        _samplePluginSmartLink.Close(Core::infinite);
    }
}

void ComRpcFixture::Echo100B(::benchmark::State& state)
{
    DoEcho(state, MessageStrings::OneHundredB);
}

void ComRpcFixture::Echo1K(::benchmark::State& state)
{
    DoEcho(state, MessageStrings::OneKB);
}

void ComRpcFixture::Echo10K(::benchmark::State& state)
{
    DoEcho(state, MessageStrings::TenKB);
}

void ComRpcFixture::Echo100K(::benchmark::State& state)
{
    DoEcho(state, MessageStrings::OneHundredKB);
}

void ComRpcFixture::Echo1M(::benchmark::State& state)
{
    DoEcho(state, MessageStrings::OneMB);
}

void ComRpcFixture::DoEcho(::benchmark::State& state, const std::string& msg)
{
    if (_samplePluginSmartLink.IsOperational()) {
        std::string result;
        auto success = _samplePluginSmartLink.Interface()->Echo(msg, result);

        if (success != Core::ERROR_NONE) {
            std::string errorMsg = "COM-RPC returned error " + std::to_string(success) + " (" + std::string(Core::ErrorToString(success)) + ")";
            state.SkipWithError(errorMsg.c_str());
        }

    } else {
        state.SkipWithError("SamplePlugin not operational");
    }
}