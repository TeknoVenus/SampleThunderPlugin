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
    Log("JSON-RPC Simple Test App");

    // Must tell the JSON-RPC client where to find Thunder
    const std::string thunderUrl = "127.0.0.1:55555";
    Core::SystemInfo::SetEnvironment(_T("THUNDER_ACCESS"), (_T(thunderUrl.c_str())));
    Log("Attempting to connect to Thunder at %s", thunderUrl.c_str());

    {
        JSONRPC::SmartLinkType<Core::JSON::IElement> remoteObject(_T("SamplePlugin.1"), _T("client.jsonrpc.2"));

        // Wait 1 second for the JSON-RPC link to be established
        // See other JSON-RPC example for a better way to do this
        SleepS(1);

        // Build our message to send over JSON-RPC
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
    }

    // Always call this at the end to clean up
    Core::Singleton::Dispose();

    return EXIT_SUCCESS;
}