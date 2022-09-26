# TestApps
Two test apps for testing the plugin

## JSONRPC
Invokes the `Greeter` method both synchronously and async over JSON-RPC. Connects to Thunder via a WebSocket link

```shell
vagrant@dobby-vagrant-focal:~/srcThunder/build/samplethunderplugin/TestApps/JSONRPC$ ./JSONRPCTestApp 
Sample JSON-RPC Test App
Calling 'greeter' function synchronously
Call to SamplePlugin JSON-RPC succeeded
Greeting: Good Afternoon, World
Calling 'greeter' function asynchronously
Waiting for callback
Callback triggered! Got greeting Goodbye, World
```

## COMRPC
Invokes the `Greeter` method over COM-RPC via the `/tmp/communicator` unix socket.

```shell
vagrant@dobby-vagrant-focal:~/srcThunder/build/samplethunderplugin/TestApps/COMRPC$ ./COMRPCTestApp 
[main.cpp:6](main): Starting SamplePluginClient
[main.cpp:7](main): Constructing SamplePlugin Client Library. . .
[SamplePluginClient.cpp:49](SamplePluginClient): Connecting to Thunder @ '/tmp/communicator'
[SamplePluginClient.cpp:21](SomethingHappend): Received an exciting notification!
[main.cpp:16](main): Generated greeting - Hello, Foobar
```