#include "SamplePluginClient.h"
#include <memory>

int main(int argc, char const *argv[])
{
    Log("Starting SamplePluginClient");

    std::unique_ptr<SamplePluginClient> client = std::make_unique<SamplePluginClient>();

    if (client->IsValid())
    {
        std::string greeting = client->GetGreeting("Foobar");
        Log("Generated greeting - %s", greeting.c_str());

        // The plugin sends out regular notifications. The client library has a callback
        // when notifications fire, so for a demo wait so we can prove they're working
        Log("Waiting for notifications. . .");
        std::this_thread::sleep_for(std::chrono::seconds(10));

        client->DeactivateSamplePlugin();

    }

    return 0;
}
