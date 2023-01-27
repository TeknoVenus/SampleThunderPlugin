# Test Apps
Test client apps for testing the sample plugin. These provide examples of how you could communicate with Thunder from your C++ application. A number of examples are provided for both JSON and COM-RPC communication.

Note since Thunder is built into seperate component libraries, there is no need to link against the entire of WPEFramework. Instead, these test apps will only link to the essential components required for that communication protocol.

## JSONRPC (Simple)
Invokes the `Greet` method over JSON-RPC. Connects to Thunder via a WebSocket link (default to port 55555)

```shell
[main.cpp:33](main): JSON-RPC Test App
[main.cpp:38](main): Attempting to connect to Thunder at 127.0.0.1:55555
[main.cpp:46](main): SamplePlugin is activated
[main.cpp:57](main): Successfully generated greeting: Hi, World
```

## JSONRPC
Invokes the `Greet` method over JSON-RPC. Uses a custom SmartLink to extend the base functionality

```shell
[main.cpp:31](main): JSON-RPC Test App
[main.cpp:36](main): Attempting to connect to Thunder at 127.0.0.1:55555
[main.cpp:54](main): Successfully generated greeting: Hello, World
```


## COMRPC (Simple)
Invokes the `Greet` method over COM-RPC via the `/tmp/communicator` unix socket. Uses the Smart Link to gracefully handle crashes and exceptions.

```shell
[main.cpp:28](main): Starting COM-RPC Simple Test App
[main.cpp:43](main): Generated greeting 'Good Afternoon, World'
```

## COMRPC
A more involved COM-RPC example that creates a custom SmartLink type to extend the base functionality.

Demonstrates connecting to SamplePlugin over a custom COM-RPC server, and shows how we could handle receiving notifications over COM-RPC

```shell
[main.cpp:27](main): Starting COM-RPC Test App
[SamplePluginLink.cpp:20](SamplePluginLink): Constructing SamplePluginLink for callsign SamplePlugin
[SamplePluginLink.cpp:31](SamplePluginLink): Connecting on custom com-rpc socket /tmp/SamplePlugin
[SamplePluginLink.cpp:60](Operational): Operational state has changed: Up
[SamplePluginLink.cpp:36](SamplePluginLink): Connected to Thunder over COM-RPC
[main.cpp:62](main): Trying to call greeter method
[SamplePluginLink.cpp:8](SomethingHappend): The "Something happened" notification fired
[main.cpp:67](main): Generated greeting 'Good Morning, World'
[SamplePluginLink.cpp:60](Operational): Operational state has changed: Down
```