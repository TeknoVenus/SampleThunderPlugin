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

#include <chrono>
#include <condition_variable>
#include <mutex>

using namespace WPEFramework;
using namespace JsonData::SamplePlugin;

using SmartLink = JSONRPC::SmartLinkType<Core::JSON::IElement>;

class SamplePluginLink : public SmartLink {
public:
    SamplePluginLink(const string& callsign, const TCHAR* clientId)
        : SmartLink(callsign, clientId)
    {
    }
    ~SamplePluginLink() = default;
    SamplePluginLink(const SamplePluginLink&) = delete;
    SamplePluginLink& operator=(const SamplePluginLink&) = delete;

    void StateChange() override
    {
        _establishedCondition.notify_one();
    }

    /**
     * Block and wait for the JSON-RPC link to be
     * established with Thunder
     */
    bool WaitForLinkEstablish(int timeoutMs)
    {
        std::unique_lock<std::mutex> lock(_lock);
        auto result = _establishedCondition.wait_for(lock, std::chrono::milliseconds(timeoutMs));
        return result != std::cv_status::timeout;
    }

private:
    std::mutex _lock;
    std::condition_variable _establishedCondition;
};

int main(int argc, char const* argv[])
{
    {
        Log("Sample JSON-RPC Test App");
        Core::SystemInfo::SetEnvironment(_T("THUNDER_ACCESS"), (_T("127.0.0.1:55555")));

        SamplePluginLink remoteObject(_T("SamplePlugin.1"), _T("client.jsonrpc.2"));

        if (remoteObject.WaitForLinkEstablish(2000)) {
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
        } else {
            Log("Timeout waiting for link to establish");
        }
    }

    Core::Singleton::Dispose();

    return EXIT_SUCCESS;
}