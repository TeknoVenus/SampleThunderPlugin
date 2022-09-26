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
#include "../Log.h"
#include <interfaces/json/JsonData_SamplePlugin.h>

using namespace WPEFramework;
using namespace JsonData::SamplePlugin;

#define TIMEOUT_MS 2000

void SomethingHappenedNotification(SomethingHappendParamsData& event)
{
    string eventData;
    event.Event.ToString(eventData);

    Log("'Something Happened' notification triggered with event data %s", eventData.c_str());
}

int main(int argc, char const* argv[])
{
    Log("Sample JSON-RPC Test App");
    Core::SystemInfo::SetEnvironment(_T("THUNDER_ACCESS"), (_T("127.0.0.1:9998")));

    // Establish a connection with Thunder over JSON-RPC websocket
    // Need to specify the plugin we want to connect to, and an ID for ourselves
    JSONRPC::SmartLinkType<Core::JSON::IElement> remoteObject(_T("SamplePlugin.1"), _T("jsonrpc.testapp"));

    // Subscribe to events
    remoteObject.Subscribe<SomethingHappendParamsData>(TIMEOUT_MS, "somethinghappend", SomethingHappenedNotification);

    if (!remoteObject.IsActivated()) {
        Log("SamplePlugin not activated");
    } else {
        Log("Calling 'SamplePlugin.1.Greeter' method");

        GreetParamsData params;
        params.Message = "World";
        Core::JSON::String result;

        uint32_t status = remoteObject.Invoke<GreetParamsData, Core::JSON::String>(TIMEOUT_MS, "greeter", params, result);

        if (status == Core::ERROR_NONE) {
            Log("Call to SamplePlugin JSON-RPC succeeded. Greeting: %s", result.Value().c_str());
        } else {
            Log("Error %u (%s)", status, Core::ErrorToString(status));
        }
    }

    return EXIT_SUCCESS;
}
