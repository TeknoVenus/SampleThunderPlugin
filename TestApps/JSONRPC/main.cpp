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

#include "../Log.h"
#include "Module.h"

// Our auto-generated JSON objects
#include <interfaces/json/JsonData_SamplePlugin.h>

using namespace WPEFramework;
using namespace JsonData::SamplePlugin;

int main(int argc, char const* argv[])
{
    {
        Log("Sample JSON-RPC Test App");
        Core::SystemInfo::SetEnvironment(_T("THUNDER_ACCESS"), (_T("127.0.0.1:55555")));

        JSONRPC::SmartLinkType<Core::JSON::IElement> remoteObject(_T("SamplePlugin.1"), _T("client.jsonrpc.2"));

        // SmartLink ctor will query Controller for the status of the plugin and subscribe for the statechange
        // event. That all happens async and there's currently no easy way to know when this has completed
        // so for now add a sleep before we check if the plugin is activated
        SleepS(2);

        if (remoteObject.IsActivated()) {
            Log("SamplePlugin is activated");

            GreetParamsData params;
            params.Message = "World";
            Core::JSON::String result;

            // Call the greeter method
            auto success = remoteObject.Invoke<GreetParamsData, Core::JSON::String>(5000, "greet", params, result);

            if (success == Core::ERROR_NONE) {
                Log("Successfully generated greeting: %s", result.Value().c_str());
            } else {
                Log("Failed to generate greeting with error %s", Core::ErrorToString(success));
            }
        } else {
            Log("SamplePlugin is deactivated");
        }
    }

    Core::Singleton::Dispose();

    return EXIT_SUCCESS;
}