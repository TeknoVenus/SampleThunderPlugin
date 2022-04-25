#pragma once

// The module name is used for trace control - every plugin should have a unique module name
// to allow for runtime control of plugin log levels
#ifndef MODULE_NAME
#define MODULE_NAME Plugin_SamplePlugin
#endif

#include <plugins/plugins.h>
#include <tracing/tracing.h>

#undef EXTERNAL
#define EXTERNAL
