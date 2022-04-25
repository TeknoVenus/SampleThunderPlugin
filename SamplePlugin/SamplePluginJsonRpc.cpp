#include "SamplePlugin.h"

#define SAMPLEPLUGIN_METHOD_GREETER "greeter"

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
        }

        /**
         * Unregister all our JSON-RPC methods
         */
        void SamplePlugin::UnregisterAllMethods()
        {
            JSONRPC::Unregister(_T(SAMPLEPLUGIN_METHOD_GREETER));
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
    }
}