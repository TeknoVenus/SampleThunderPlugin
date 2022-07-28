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

#pragma once

#include <benchmark/benchmark.h>

#include "Module.h"
#include <interfaces/ISamplePlugin.h>
#include <WPEFramework/interfaces/json/JsonData_SamplePlugin.h>
#include <memory.h>

using namespace WPEFramework;
using namespace JsonData::SamplePlugin;

class JsonRpcFixture : public benchmark::Fixture
{
public:
    void SetUp(const ::benchmark::State &state);
    void TearDown(const ::benchmark::State &state);

    void Echo100B(::benchmark::State &state);
    void Echo1K(::benchmark::State &state);
    void Echo10K(::benchmark::State &state);
    void Echo100K(::benchmark::State &state);
    void Echo1M(::benchmark::State &state);

private:
    void DoEcho(::benchmark::State &state, const std::string& msg);

private:
    std::unique_ptr<JSONRPC::LinkType<Core::JSON::IElement>> mRemoteObject;
};