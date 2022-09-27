#pragma once

#include "Module.h"

using namespace WPEFramework;

/**
 * JSON container for custom config options we want to use for
 * this plugin
 *
 * Always use strongly typed JSON containers of the weakly-typed
 * JsonObject!
 */
class SamplePluginConfiguration : public Core::JSON::Container {
public:
    SamplePluginConfiguration()
        : Core::JSON::Container()
        , Greetings()
    {
        // Map a json object name to c++ object
        Add(_T("greetings"), &Greetings);
    }
    ~SamplePluginConfiguration() = default;

    SamplePluginConfiguration(const SamplePluginConfiguration&) = delete;
    SamplePluginConfiguration& operator=(const SamplePluginConfiguration&) = delete;

public:
    Core::JSON::ArrayType<Core::JSON::String> Greetings;
};