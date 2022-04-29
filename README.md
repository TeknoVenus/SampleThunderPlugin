# Sample Thunder Plugin
A simple Thunder plugin for experimenting and learning about Thunder.

Has the following features
* JSON-RPC API
* COM-RPC API
* Can run in-process and out-of-process
* Contains accompanying client library and test application

Should not be taken as the "best" way to write a plugin, but as a way of exploring the various ways plugins can be created.

# Build
Set up a build environment that can build Thunder. This requires the following Thunder repos:

* Thunder
* ThunderInterfaces

Then clone this repo.

You should end up with a directory structure as below

```
~/srcThunder/
├── TestPlugins
├── Thunder
├── ThunderInterfaces
```

* Copy the `TestPlugins/Interfaces/ISamplePlugin.h` interface to `ThunderInterfaces/interfaces`. This is the COM-RPC API we implement
* Copy the `TestPlugins/Interfaces/SamplePlugin.json` schema to `ThunderInterfaces/jsonrpc`. This is the JSON-RPC API we implement

Now build Thunder (only need to do this once). In all below instructions, run the commands in the root directory you cloned all the repos into

```shell
mkdir ./build

# Thunder Tools
sudo cmake -HThunder/Tools -Bbuild/ThunderTools -DCMAKE_INSTALL_PREFIX=/usr
sudo make -j4 -C build/ThunderTools && make -C build/ThunderTools install

# Thunder Core
sudo cmake -HThunder -Bbuild/Thunder -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_TYPE=Debug -DBINDING=127.0.0.1 -DPORT=9998

sudo make -j4 -C build/Thunder && make -C build/Thunder install
```

Now build Thunder interfaces (do this every time you change `ISamplePlugin.h` or `SamplePlugin.json`)
```shell
sudo cmake -HThunderInterfaces -Bbuild/ThunderInterfaces  -DCMAKE_INSTALL_PREFIX=/usr
sudo make -j4 -C build/ThunderInterfaces && make -C build/ThunderInterfaces install
```

Finally build the sample plugin & accompanying client libraries and test app
```
sudo cmake -HTestPlugins -Bbuild/TestPlugins -DCMAKE_INSTALL_PREFIX=/usr
sudo make -j4 -C build/TestPlugins && make -C build/TestPlugins install
```

# Usage
First, start WPEFramework by running `/usr/bin/WPEFramework`. You should see SamplePlugin autostart

```
[     116698 us] Activating plugin [SamplePlugin]:[SamplePlugin]
[     117632 us] Initializing SamplePlugin
[     117668 us] Initialize running in process 36077
[     128978 us] Activated plugin [SamplePlugin]:[SamplePlugin]
```

## Enable SamplePlugin tracing
By default, WPEFramework does not enable tracing for plugins. Enable all trace levels for SamplePlugin by making the following JSON-RPC request

```
curl --request POST \
  --url http://localhost:9998/jsonrpc \
  --header 'Content-Type: application/json' \
  --data '{
	"jsonrpc": "2.0",
	"id": 1234567890,
	"method": "TraceControl.set",
	"params": {
		"module": "Plugin_SamplePlugin",
		"state": "enabled"
	}
}'
```

## JSON-RPC
Make the following JSON-RPC request to generate a greeting

```
curl --request POST \
  --url http://localhost:9998/jsonrpc \
  --header 'Content-Type: application/json' \
  --data '{
	"jsonrpc": "2.0",
	"id": 1234567890,
	"method": "SamplePlugin.greeter",
	"params": {
		"message": "world"
	}
}'
```
The plugin should respond with a generated greeting
```json
{
	"jsonrpc": "2.0",
	"id": 1234567890,
	"result": {
		"greeting": "Hello, world",
		"success": true
	}
}
```

You should see the below logs in WPEFramework
```
[Mon, 25 Apr 2022 12:03:40 ]:[SamplePluginJsonRpc.cpp:47] Information: Incoming JSON-RPC request for greeter method with input params {"message":"world"}
[Mon, 25 Apr 2022 12:03:40 ]:[SamplePlugin.cpp:150] Information: Generating greeting
[Mon, 25 Apr 2022 12:03:40 ]:[SamplePlugin.cpp:151] Information: Running in process 36087
[Mon, 25 Apr 2022 12:03:40 ]:[SamplePlugin.cpp:137] Information: Raising a notification
```

## COM-RPC
To test COM-RPC, use the test client and accompanying example C++ library.

For test purposes, the test client will activate & deactivate the plugin when it starts/exits respectively.

```shell
$ cd ./build/TestPlugins/SamplePluginClientLibrary
$ ./SamplePluginClient 
[main.cpp:6](main): Starting SamplePluginClient
[SamplePluginClient.cpp:22](SamplePluginClient): Connected to Thunder @ '/tmp/communicator'
[SamplePluginClient.cpp:34](SamplePluginClient): SamplePlugin is deactivated - activating now
[SamplePluginClient.cpp:43](SamplePluginClient): Activated plugin successfully in 12 ms
[main.cpp:12](main): Generated greeting - Hello, Stephen
[SamplePluginClient.cpp:66](~SamplePluginClient): Deactivating plugin
[SamplePluginClient.cpp:75](~SamplePluginClient): Deactivated plugin successfully in 1 ms
```

## Switch between In-Process and Out-Of-Process execution
To change if the plugin runs in or out-of-process, edit the `SamplePlugin.config` file in `TestPlugins/SamplePlugin/SamplePlugin.config` and rebuild. Then restart WPEFramework.

When running out-of-process, you'll see a new WPEProcess spawned when the plugin is activated
```
vagrant    36077  0.0  0.5 618988 21904 pts/1    Sl+  12:00   0:00  |           \_ WPEFramework
vagrant    37676  0.0  0.3  95684 14360 pts/1    Sl+  12:07   0:00  |               \_ WPEProcess -l libWPEFrameworkSamplePlugin.so -c SamplePlugin -C SamplePlugin -r /tmp/communicator -i 224 -x 5 -p "/root/SamplePlugin/" -s "/usr/lib/wpeframework/plugins/" -d "/usr/share/WPEFramework/SamplePlugin/" -a "/usr/bin/" -v "/tmp/SamplePlugin/" -m "/usr/lib/wpeframework/proxystubs/" -P "/opt/minidumps/" -e 127
```
