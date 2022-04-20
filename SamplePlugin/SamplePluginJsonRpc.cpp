#include "SamplePlugin.h"

namespace WPEFramework
{
    namespace Plugin
    {
        using namespace JsonData::SamplePlugin;

        void SamplePlugin::RegisterAllMethods()
        {
            Register<GreeterParamsData, GreeterResultData>(_T("greeter"), &SamplePlugin::Greeter, this);
        }

        void SamplePlugin::UnregisterAllMethods()
        {
            Unregister(_T("greeter"));
        }

        uint32_t SamplePlugin::Greeter(const GreeterParamsData& params, GreeterResultData& response)
        {
            uint32_t errorCode = Core::ERROR_NONE;

            printf("Greeter JSON-RPC\n");

            // Validate incoming JSON data
            if (params.Message.IsNull())
            {
                printf("Must provide a message!\n");
                return Core::ERROR_BAD_REQUEST;
            }

            std::string result;
            errorCode = _samplePlugin->Greet(params.Message.Value(), result);

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
            return 0;
        }
    }
}