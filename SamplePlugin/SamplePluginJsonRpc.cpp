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

#include "SamplePlugin.h"

#define SAMPLEPLUGIN_METHOD_GREETER "greeter"
#define SAMPLEPLUGIN_METHOD_ECHO "echo"

namespace WPEFramework
{
    namespace Plugin
    {
        using namespace JsonData::SamplePlugin;

        /**
         * Hook up all our JSON RPC methods
         *
         * Each method definition comprises of:
         *  * Input parameters
         *  * Output parameters
         *  * Method name
         *  * Function that implements that method
         */
        void SamplePlugin::RegisterAllMethods()
        {
            JSONRPC::Register<GreeterParamsData, GreeterResultData>(_T(SAMPLEPLUGIN_METHOD_GREETER), &SamplePlugin::Greeter, this);
            JSONRPC::Register<EchoParamsData, EchoResultData>(_T(SAMPLEPLUGIN_METHOD_ECHO), &SamplePlugin::Echo, this);
        }

        /**
         * Unregister all our JSON-RPC methods
         */
        void SamplePlugin::UnregisterAllMethods()
        {
            JSONRPC::Unregister(_T(SAMPLEPLUGIN_METHOD_GREETER));
            JSONRPC::Unregister(_T(SAMPLEPLUGIN_METHOD_ECHO));
        }

        /**
         * JSON-RPC wrapper around the Greeter() method the plugin implements
         *
         * Do no actual work here, this should just deal with validating the incoming JSON, then
         * call out to the actual implementation over COM-RPC, then build the response JSON
         *
         */
        uint32_t SamplePlugin::Greeter(const GreeterParamsData &params, GreeterResultData &response)
        {
            uint32_t errorCode = Core::ERROR_NONE;

            // For debugging
            std::string paramsString;
            params.ToString(paramsString);
            TRACE(Trace::Information, (_T("Incoming JSON-RPC request for greeter method with input params %s"), paramsString.c_str()));

            // Validate incoming JSON data
            if (!params.Message.IsSet() || params.Message.Value().empty())
            {
                TRACE(Trace::Error, (_T("Message cannot be null")));
                return Core::ERROR_BAD_REQUEST;
            }

            // Invoke the actual method to do the work
            std::string result;
            errorCode = _samplePlugin->Greet(params.Message.Value(), result);

            // Build the response JSON object
            if (errorCode == Core::ERROR_NONE)
            {
                response.Success = true;
                response.Greeting = result;
            }
            else
            {
                response.Success = false;
            }

            return errorCode;
        }


        uint32_t SamplePlugin::Echo(const  EchoParamsData &params, EchoResultData &response)
        {
            uint32_t errorCode = Core::ERROR_NONE;

            // Validate incoming JSON data
            if (!params.Data.IsSet() || params.Data.Value().empty())
            {
                TRACE(Trace::Error, (_T("Message cannot be null")));
                return Core::ERROR_BAD_REQUEST;
            }

            // Invoke the actual method to do the work
            std::string result;
            errorCode = _samplePlugin->Echo(params.Data.Value(), result);

            // Build the response JSON object
            if (errorCode == Core::ERROR_NONE)
            {
                response.Success = true;
                response.Data = result;
            }
            else
            {
                response.Success = false;
            }

            return errorCode;
        }
    }
}