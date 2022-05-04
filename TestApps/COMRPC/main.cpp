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
