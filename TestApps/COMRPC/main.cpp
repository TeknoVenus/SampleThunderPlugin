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


#include "../../SamplePluginClientLibrary/SamplePluginClient.h"
#include <memory>

int main(int argc, char const *argv[])
{
    Log("Starting SamplePluginClient");
    Log("Constructing SamplePlugin Client Library. . .");

    std::unique_ptr<SamplePluginClient> client = std::make_unique<SamplePluginClient>();

    // Constructing SamplePluginClient will automatically activate the plugin
    // if it's not already activated
    if (client->IsValid())
    {
        std::string greeting = client->GetGreeting("Foobar");
        Log("Generated greeting - %s", greeting.c_str());

        //client->DeactivateSamplePlugin();
    }

    return 0;
}
