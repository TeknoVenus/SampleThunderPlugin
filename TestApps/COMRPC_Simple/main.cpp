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
#include <interfaces/ISamplePlugin.h>

using namespace WPEFramework;

int main(int argc, char const* argv[])
{
    Log("Starting COM-RPC Simple Test App");

    {
        RPC::SmartInterfaceType<Exchange::ISamplePlugin> samplePluginSmartLink;

        // Attempt to connect to Thunder over COM-RPC
        auto success = samplePluginSmartLink.Open(RPC::CommunicationTimeOut, samplePluginSmartLink.Connector(), "SamplePlugin");

        if (success == Core::ERROR_NONE) {
            // Connected - check if the plugin is in a suitable state to accept requests
            if (samplePluginSmartLink.IsOperational()) {
                // Everything looks good - lets try and call a method on the plugin
                auto samplePlugin = samplePluginSmartLink.Interface();

                if (samplePlugin != nullptr) {
                    string result;
                    auto success = samplePlugin->Greet("World", result);

                    if (success == Core::ERROR_NONE) {
                        Log("Generated greeting '%s'", result.c_str());
                    } else {
                        Log("Failed to generate greeting with error %s", Core::ErrorToString(success));
                    }

                    samplePlugin->Release();
                }

            } else {
                Log("SamplePlugin is not operational - is it activated?");
            }

            // Remember to close the link when we're done
            samplePluginSmartLink.Close(Core::infinite);

        } else {
            Log("Failed to open connection to Thunder with error %d (%s)", success, Core::ErrorToString(success));
        }
    }

    // Always remember to do this before you terminate your app so Thunder
    // resources are freed
    Core::Singleton::Dispose();

    return 0;
}
