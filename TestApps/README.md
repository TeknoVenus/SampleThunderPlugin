# TestApps
Two test apps for testing the sample plugin

## JSONRPC
Invokes the `Greet` method over JSON-RPC. Connects to Thunder via a WebSocket link (default to port 55555)

```shell
/home/stephen.foulds/Thunder/build/SampleThunderPlugin/TestApps/JSONRPC/JSONRPCTestApp
[main.cpp:32](main): Sample JSON-RPC Test App
[main.cpp:43](main): SamplePlugin is activated
[main.cpp:53](main): Successfully generated greeting: Hey, World
```

## COMRPC
Invokes the `Greet` method over COM-RPC via the `/tmp/communicator` unix socket. Uses the Smart Link to gracefully handle crashes and exceptions.

```shell
$ /home/stephen.foulds/Thunder/build/SampleThunderPlugin/TestApps/COMRPC/COMRPCTestApp
[main.cpp:29](main): Starting SamplePluginClient
[SamplePluginLink.cpp:20](SamplePluginLink): Constructing SamplePlugin link to callsign SamplePlugin
[SamplePluginLink.cpp:62](Operational): Operational state has changed: Up
[main.cpp:57](main): Successfully activated the plugin
[main.cpp:61](main): Trying to call greeter method
[SamplePluginLink.cpp:7](SomethingHappend): The "Something happened" notification fired
[main.cpp:69](main): Generated greeting Good Afternoon, World
[main.cpp:75](main): Deactivating plugin
[SamplePluginLink.cpp:62](Operational): Operational state has changed: Down
[main.cpp:81](main): Successfully deactivated the plugin
```