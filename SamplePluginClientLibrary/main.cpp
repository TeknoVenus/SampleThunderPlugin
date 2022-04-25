#include "SamplePluginClient.h"
#include <memory>

int main(int argc, char const *argv[])
{
    Log("Starting SamplePluginClient");

    std::unique_ptr<SamplePluginClient> client = std::make_unique<SamplePluginClient>();

    std::string greeting = client->GetGreeting("Foobar");

    Log("Generated greeting - %s", greeting.c_str());

    return 0;
}
