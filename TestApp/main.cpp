#include "Module.h"

#include <websocket/websocket.h>

#include <WPEFramework/interfaces/ISamplePlugin.h>
#include <WPEFramework/interfaces/json/JsonData_SamplePlugin.h>

#include <stdio.h>
#include <memory>
#include <unistd.h>

#include <mutex>
#include <future>
#include <functional>

using namespace WPEFramework;
using namespace JsonData::SamplePlugin;

typedef std::function<void(const GreeterResultData &)> CallbackFn;

static std::promise<void> promise;

namespace Handlers
{
    class Callbacks
    {
    public:
        void async_callback_complete(const GreeterResultData &response, const Core::JSONRPC::Error *result)
        {
            printf("Callback triggered! Got greeting %s\n", response.Greeting.Value().c_str());
            promise.set_value();
        }
    };
}

int main(int argc, char const *argv[])
{
    Handlers::Callbacks testCallback;

    printf("Creating JSON-RPC link\n");
    Core::SystemInfo::SetEnvironment(_T("THUNDER_ACCESS"), (_T("127.0.0.1:55555")));
    //auto remoteObject = std::make_unique<JSONRPC::LinkType<Core::JSON::IElement>>("SamplePlugin.1", "");
    auto remoteObjectMsgPack = std::make_unique<JSONRPC::LinkType<Core::JSON::IMessagePack>>("SamplePlugin.1", "");

    GreeterParamsData params;
    params.Message = "World";
    GreeterResultData result;

    //remoteObject->Invoke<GreeterParamsData, GreeterResultData>(2000, "greeter", params, result);
    remoteObjectMsgPack->Invoke<GreeterParamsData, GreeterResultData>(2000, "greeter", params, result);


    // uint32_t status = remoteObjectMsgPack->Invoke<GreeterParamsData, GreeterResultData>(2000, "greeter", params, result);

    // if (status == Core::ERROR_NONE)
    // {
    //     printf("Call to SamplePlugin JSON-RPC succeeded\n");

    //     printf("Greeting: %s\n", result.Greeting.Value().c_str());
    // }
    // else
    // {
    //     printf("Error %u (%s)\n", status, Core::ErrorToString(status));
    // }

    // std::future<void> future = promise.get_future();
    // remoteObjectMsgPack->Dispatch<GreeterParamsData>(30000, "greeter", params, &Handlers::Callbacks::async_callback_complete, &testCallback);

    // printf("Waiting for callback\n");
    // future.wait();

    return 0;
}
