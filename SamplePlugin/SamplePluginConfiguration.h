#pragma once

#include "Module.h"

using namespace WPEFramework;

class SamplePluginConfiguration : public Core::JSON::Container {
public:
    SamplePluginConfiguration()
        : Core::JSON::Container()
        , Greetings()
    {
        Add(_T("greetings"), &Greetings);
    }
    ~SamplePluginConfiguration() = default;

    SamplePluginConfiguration(const SamplePluginConfiguration&) = delete;
    SamplePluginConfiguration& operator=(const SamplePluginConfiguration&) = delete;

public:
    Core::JSON::ArrayType<Core::JSON::String> Greetings;
};