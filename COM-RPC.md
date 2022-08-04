# Introduction to COM-RPC
For internal and high-performance APIs, Thunder offers an alternative IPC/RPC mechanism called COM-RPC. This is an internally developed IPC protocol inspired by Microsoft COM, designed to provide a native C/C++ interface that can be used to communicate transparently across process boundaries. By default it uses unix domain sockets, although can work over other socket types (TCP, UDP) on platforms that don't support unix sockets (i.e. Windows).

A COM-RPC interface is a requirement for a plugin to run outside the main WPEFramework daemon since communication with the main Thunder daemon and WPEProcess host is performed over COM-RPC. Thunder provides the following execution models:

* Out-of-process: A WPEProcess host process is launched as a child of the WPEFramework daemon
* Container: The WPEProcess host is launched inside a container (supports LXC, runc, crun and Dobby depending on Thunder build configuration).

The execution mode of the plugin can be changed at runtime without rebuilding.

# Interfaces
To support COM-RPC, a plugin must define a COM-RPC interface. This is done by creating C++ struct that inherits from Core::IUnknown. This IUnknown interface indicates the interface will be used across process boundaries.

COM-RPC interfaces will then define the methods the plugin must implement. Whilst these methods can be anything, convention is for the methods to return a uint32_t error code (Core::ERROR_NONE in a success scenario), and return any data in out parameters.

All public Thunder COM-RPC interfaces live in the ThunderInterfaces repo on GitHub: https://github.com/rdkcentral/ThunderInterfaces/

## Design Guidelines

* Each interface inheriting from Core::IUnknown must have a unique ID
* Ensure backwards compatibility is kept to avoid breaking existing API consumers
* Methods must be pure virtual methods that can be overridden by the plugin implementation
* Methods should return uint32_t which will store an error code if the method failed, or Core::ERROR_NONE if the method call succeeded
  * If necessary, data should be returned in an out parameter
* COM-RPC interfaces can cross process and even host boundaries, so it is feasible that the COM-RPC interface could cross architectures (32bit and 64bit). As such:
  * Ensure any enums have explicit data types set (e.g. uint8_t)
  * Avoid passing data in/out of the API in structs (which may be packed/aligned differently depending on architecture/compiler)
  * Prefer using scalar values, avoid using C++ types such as std::vector and std::map.

During configure time, Thunder will generate Proxy Stubs for every COM-RPC interface. These stubs can mostly be ignored from a developer perspective as you do not interact with them directly. They are only used for internal COM-RPC communication.

## Interface Registration
Plugins that implement a COM-RPC interface must build an interface map to ensure Thunder is aware of the implemented interfaces.

```c++
BEGIN_INTERFACE_MAP(SamplePlugin)
INTERFACE_ENTRY(PluginHost::IPlugin)
INTERFACE_ENTRY(PluginHost::IDispatcher)
INTERFACE_ENTRY(Exchange::ISamplePlugin)
END_INTERFACE_MAP
```

If the interface is realised in a seperate class/library, this can be done with an aggregate

```c++
BEGIN_INTERFACE_MAP(SamplePlugin)
INTERFACE_ENTRY(PluginHost::IPlugin)
INTERFACE_ENTRY(PluginHost::IDispatcher)
INTERFACE_AGGREGATE(Exchange::ISamplePlugin, _samplePlugin)
END_INTERFACE_MAP
```

Here SamplePlugin implements the IPlugin, IDispatcher (necessary for JSON-RPC support) and ISamplePlugin interfaces.

### Plugin Initialization
During the plugin initialization, the plugin must announce it's interfaces to signal to Thunder that it has started and the interfaces can be queried

```c++
service->Root<Exchange::ISamplePlugin>(_connectionId, 2000, _T("SamplePlugin"));
```

Calling `Root<>(...)` does a number of things:

If the plugin is in-process
* Locate the library .so file that implements the COM-RPC interface (this may be separate from the main plugin .so)
* Load the .so file
* Query the interfaces provided by the .so (ensure it implements the interface we asked it to register)

If the plugin is out-of-process
* Locate the library .so file that implements the COM-RPC interface this may be separate from the main plugin .so)
* Create an RPC object to store info about the to-be-launched proces
* Create a RemoteConnection to track the connetion between WPEFramework and WPEProcess
* Build the argument list and launch WPEProcess
* WPEProcess queries the interfaces provided by the .so (ensure it implements the inerface we asked it to register)
* Wait for WPEProcess to send an announcement message over COM-RPC that it has started

# Important Note for Out-Of-Process Plugins
It may not be obvious, but when a plugin runs out of process, the plugin library is actually loaded in two places:

* Inside WPEFramework daemon
  * Initialize() and Deinitialize() run inside the main daemon
    * Initialize is responsible for spawning the out-of-process component by calling `Root<>(...)`
  * All JSON-RPC methods are handled inside the main WPEFramework daemon
    * JSON-RPC methods should call out to COM-RPC methods (running out-of-process) to do actual work
* In WPEProcess
    * This loads the plugin again and registers the plugin's COM-RPC interface
    * Any calls over COM-RPC will be executed in this process

As a result, it is important to ensure all work is performed behind the COM-RPC interface, since the goal is for all work to be done in the out-of-process portion of the plugin. Any JSONRPC APIs should ideally be wrappers around the COM-RPC interfaces.

It is possible to split a plugin into two libraries (e.g `SamplePlugin.so` and `SamplePluginImplementation.so`), so that the main library is loaded inside WPEFramework and the second library is loaded in WPEProcess only. This is something Liberty Global are implementing for their Thunder deployment https://wiki.rdkcentral.com/pages/viewpage.action?pageId=198262351

# Consuming COM-RPC APIs
For consumers of COM-RPC APIs, apps can either directly interface with the plugin over COM-RPC or the plugin author can develop a "client library" that abstracts the COM-RPC internals away from the consumer. Thunder provides a few client libraries out of the box, the most commonly used being OCDM and SecurityAgent: https://github.com/rdkcentral/ThunderClientLibraries

COM-RPC communication is done (by default) over the `/tmp/communicator` socket on Linux. Note all COM-RPC communication flows over this socket, unless a plugin starts their own custom COM-RPC server for peer-to-peer connections. This is not documented here, but can be found in the OCDM plugin as a reference.

# Notifications/Events
It is possible for plugins to generate notifications/events over COM-RPC, which consuming apps can subscribe to and fire a callback when the notification occurs.

Plugins should define a notification interface (typically called something like INotification) as part of the COM-RPC interface. It should provide methods for each notification that can be raised.

It is up to the plugin to maintain a list of what processes have subscribed to their notifications. When it is time to raise the notification, the plugin will simply loop through all the subscribed callbacks and trigger the notification.

On the client side, the client should provide their own implementation of the INotification interface, overriding the notification methods with their own code. Then the client should register this implementation of INotification with the plugin.

# COM-RPC and JSON-RPC co-existence
It is possible (and encouraged!) for COM-RPC and JSON-RPC interfaces to co-exist in order to get the best of both worlds - easy integration and testing for web-based applications and general use and performant native interfaces for C/C++ applications.

If a plugin realises a COM-RPC interface, then a JSON-RPC wrapper can be created around that interface. This wrapper is responsible for very little other than validating the incoming JSON, calling the COM-RPC API then building up a JSON result.

