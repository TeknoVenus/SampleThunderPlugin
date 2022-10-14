# SamplePlugin
A sample Thunder plugin supporting JSONRPC and COMRPC

The plugin is split into two libraries - `libWPEFrameworkSamplePlugin.so` and `libWPEFrameworkSamplePluginImplementation.so`.

## Configuration
The plugin supports a configuration file with custom config options. The default values for this config are set in `SamplePlugin.config`, which is a [CMake++ QuickMap](https://github.com/toeb/cmakepp/blob/master/cmake/quickmap/README.md) (This is currently being phased out with a Python-based config generator but that is still a WIP).

The plugin supports the following custom config options:
* `greetings` [array of strings] - The greetings the `Greet()` method can randomly select.
* `privateComRpc` [bool] - If true, the plugin will create its own COM-RPC server on the /tmp/samplePlugin unix socket. It will expose the ISamplePlugin COM-RPC interface over this socket.

Beyond this, it supports the default Thunder plugin configuration options in the "root" object. Useful options here include:

* `locator`: The name of the library holding the plugin implementation
* `user`: Name of the user to run the out-of-process plugin under
* `group`: Name of the group to run the out-of-process plugin in
* `outofprocess`: True/False - whether to run the plugin in or out of process

More options can be found in the `IShell.h` header in Thunder source code.

## Code Structure
### `Module.cpp`/`Module.h`
All Thunder plugins must have a `Module.cpp`/`Module.h` file. This includes a number of necessary header files, defines the module name (used for trace control) and sets the build reference (i.e. git hash the plugin was built from)

### `SamplePlugin.config`
Configuration for the plugin - change settings such as whether the plugin runs in or out of process. This configuration contains an array of greetings the plugin can choose from to generate a custom greeting message.

This config file is converted into a JSON file during configure time.

### `SamplePluginConfiguration.h`
A JSON object that represents the custom configuration for this plugin.

When the plugin is loaded, the config file is read from disk and stored in this object so it can be read by the plugin code

### `SamplePlugin.cpp`/`SamplePlugin.h`
This is the code plugin code that is loaded by WPEFramework. It implements the `IPlugin` and `IDispatcher` interfaces, and provides the `Initialize()` and `Deinitialize()` methods called by Thunder when activating/deactivating the plugin.

This is built into the `libWPEFrameworkSamplePlugin.so` shared library and is always loaded into the main WPEFramework daemon, even if the plugin runs out-of-process. It is kept intentionally small so that when the plugin is running out of process, the memory footprint of WPEFramework is reduced. It should not link to any libraries other than those provided by WPEFramework

### `SamplePluginImplementation.cpp`/`SamplePluginImplementation.h`
This is where the real work is done - any business logic for the plugin should live here. If running out of process, it is this code that is actually running in the WPEProcess host outside of WPEFramework. This is built into the `libWPEFrameworkSamplePluginImplementation.so` library.

If the plugin had to link to any 3rd party libraries, they should link to this implementation library. This way the dependencies are only loaded into WPEProcess and are completely unloaded when WPEProcess exits. More info here: https://wiki.rdkcentral.com/pages/viewpage.action?pageId=198262351

This code is a concrete implementation of the API defined in the `ISamplePlugin` interface