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


#include "Module.h"

#include <websocket/websocket.h>

#include <WPEFramework/interfaces/ISamplePlugin.h>
#include <WPEFramework/interfaces/json/JsonData_SamplePlugin.h>

#include <stdio.h>
#include <memory>
#include <unistd.h>

#include <mutex>
#include <future>
#include <functional>

using namespace WPEFramework;
using namespace JsonData::SamplePlugin;

typedef std::function<void(const GreeterResultData &)> CallbackFn;

static std::promise<void> promise;

namespace Handlers
{
    class Callbacks
    {
    public:
        void async_callback_complete(const GreeterResultData &response, const Core::JSONRPC::Error *result)
        {
            printf("Callback triggered! Got greeting %s\n", response.Greeting.Value().c_str());
            promise.set_value();
        }
    };
}

int main(int argc, char const *argv[])
{
    Handlers::Callbacks testCallback;

    printf("Sample JSON-RPC Test App\n");
    Core::SystemInfo::SetEnvironment(_T("THUNDER_ACCESS"), (_T("127.0.0.1:9998")));
    auto remoteObject = std::make_unique<JSONRPC::LinkType<Core::JSON::IElement>>("SamplePlugin.1", "");

    GreeterParamsData params;
    params.Message = "World";
    GreeterResultData result;

    printf("Calling 'greeter' function synchronously\n");

    uint32_t status = remoteObject->Invoke<GreeterParamsData, GreeterResultData>(2000, "greeter", params, result);
    if (status == Core::ERROR_NONE)
    {
        printf("Call to SamplePlugin JSON-RPC succeeded\n");
        printf("Greeting: %s\n", result.Greeting.Value().c_str());
    }
    else
    {
        printf("Error %u (%s)\n", status, Core::ErrorToString(status));
        return 1;
    }

    printf("Calling 'greeter' function asynchronously\n");

    std::future<void> future = promise.get_future();
    remoteObject->Dispatch<GreeterParamsData>(30000, "greeter", params, &Handlers::Callbacks::async_callback_complete, &testCallback);

    printf("Waiting for callback\n");
    future.wait();

    return 0;
}
