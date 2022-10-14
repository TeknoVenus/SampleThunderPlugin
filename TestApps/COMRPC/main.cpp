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
#include "SamplePluginLink.h"
#include <chrono>
#include <thread>

#include <memory>

int main(int argc, char const* argv[])
{
    Log("Starting SamplePluginClient");

    /**
     * This example app will do the following:
     * - Attempt to establish a COM-RPC connection with Thunder for the SamplePlugin
     * - If the plugin is not activated, attempt to activate it
     * - If activation is successful, call one of the methods the plugin provides
     * - Deactivate the plugin
     * - Clean up
     *
     * By using the SmartInterfaceType, this is resiliant to crashes/errors
     * on the server side.
     *
     * A more realistic example client app would likely want
     * to register callbacks on the link operation (e.g. detect when the connection to the
     * server goes up/down) and take action.
     */

    {
        // For demo purposes, connect over our custom COM-RPC server
        // The socket path is optional, if not provided will default to /tmp/communicator
        SamplePluginLink samplePluginLink("SamplePlugin", "/tmp/SamplePlugin");

        // Activate the plugin if it's not already
        if (samplePluginLink.GetState() == PluginHost::IShell::state::DEACTIVATED) {
            auto success = samplePluginLink.Activate();

            if (success != Core::ERROR_NONE) {
                Log("Failed to activate the plugin with error code %s", Core::ErrorToString(success));
            } else {
                Log("Successfully activated the plugin");
            }
        }

        Log("Trying to call greeter method");
        string result;

        // By using the SmartLink, we can safely call this method knowing we will get a sensible
        // error message if the link is not operational
        auto success = samplePluginLink.Greet("World", result);

        if (success == Core::ERROR_NONE) {
            Log("Generated greeting %s", result.c_str());
        } else {
            Log("Failed to generate greeting with error %s", Core::ErrorToString(success));
        }
    }

    // Always remember to do this before you terminate your app so Thunder
    // resources are freed
    Core::Singleton::Dispose();

    return 0;
}
