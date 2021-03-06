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
├── samplethunderplugin (this repo)
├── Thunder
├── ThunderInterfaces
```

* Copy the `Interfaces/ISamplePlugin.h` interface from this repo to `ThunderInterfaces/interfaces`. This is the COM-RPC API we implement
* Copy the `Interfaces/SamplePlugin.json` schema from this repo to `ThunderInterfaces/jsonrpc`. This is the JSON-RPC API we implement
* Add a unique ID value for `ID_SAMPLE_PLUGIN` and `ID_SAMPLE_PLUGIN_NOTIFICATION` to `IDs.h` in ThunderInterfaces

Now build Thunder (only need to do this once). In all below instructions, run the commands in the root directory you cloned all the repos into

```shell
mkdir ./build

# Thunder Tools
sudo cmake -HThunder/Tools -Bbuild/ThunderTools -DCMAKE_INSTALL_PREFIX=/usr
sudo make -j4 -C build/ThunderTools && sudo make -C build/ThunderTools install

# Thunder Core
sudo cmake -HThunder -Bbuild/Thunder -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_TYPE=Debug -DBINDING=127.0.0.1 -DPORT=9998
sudo make -j4 -C build/Thunder && sudo make -C build/Thunder install
```

Now build Thunder interfaces (do this every time you change `ISamplePlugin.h` or `SamplePlugin.json`)
```shell
sudo cmake -HThunderInterfaces -Bbuild/ThunderInterfaces  -DCMAKE_INSTALL_PREFIX=/usr
sudo make -j4 -C build/ThunderInterfaces && sudo make -C build/ThunderInterfaces install
```

Finally build this repo containing the sample plugin & accompanying client libraries and test apps
```
sudo cmake -HSamplethunderplugin -Bbuild/Samplethunderplugin -DCMAKE_INSTALL_PREFIX=/usr
sudo make -j4 -C build/Samplethunderplugin && sudo make -C build/Samplethunderplugin install
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

```
[Mon, 25 Apr 2022 12:03:40 ]:[SamplePluginJsonRpc.cpp:47] Information: Incoming JSON-RPC request for greeter method with input params {"message":"world"}
[Mon, 25 Apr 2022 12:03:40 ]:[SamplePlugin.cpp:150] Information: Generating greeting
[Mon, 25 Apr 2022 12:03:40 ]:[SamplePlugin.cpp:151] Information: Running in process 36087
[Mon, 25 Apr 2022 12:03:40 ]:[SamplePlugin.cpp:137] Information: Raising a notification
You should see the below logs in WPEFramework
```

## COM-RPC
To test COM-RPC, use the test client and accompanying example C++ library.

For test purposes, the test client will activate & deactivate the plugin when it starts/exits respectively.

```shell
vagrant@dobby-vagrant-focal:~/srcThunder/build/samplethunderplugin/TestApps/COMRPC$ ./COMRPCTestApp 
[main.cpp:6](main): Starting SamplePluginClient
[main.cpp:7](main): Constructing SamplePlugin Client Library. . .
[SamplePluginClient.cpp:49](SamplePluginClient): Connecting to Thunder @ '/tmp/communicator'
[SamplePluginClient.cpp:21](SomethingHappend): Received an exciting notification!
[main.cpp:16](main): Generated greeting - Hello, Foobar
```

## Switch between In-Process and Out-Of-Process execution
To change if the plugin runs in or out-of-process, edit the `SamplePlugin.config` file in `Samplethunderplugin/SamplePlugin/SamplePlugin.config` and rebuild. Then restart WPEFramework.

When running out-of-process, you'll see a new WPEProcess spawned when the plugin is activated
```
vagrant    36077  0.0  0.5 618988 21904 pts/1    Sl+  12:00   0:00  |           \_ WPEFramework
vagrant    37676  0.0  0.3  95684 14360 pts/1    Sl+  12:07   0:00  |               \_ WPEProcess -l libWPEFrameworkSamplePlugin.so -c SamplePlugin -C SamplePlugin -r /tmp/communicator -i 224 -x 5 -p "/root/SamplePlugin/" -s "/usr/lib/wpeframework/plugins/" -d "/usr/share/WPEFramework/SamplePlugin/" -a "/usr/bin/" -v "/tmp/SamplePlugin/" -m "/usr/lib/wpeframework/proxystubs/" -P "/opt/minidumps/" -e 127
```

## Benchmarks
A benchmarking app is included to analyse the performance of COM-RPC and JSON-RPC

To build the benchmarking app, first install Google Benchmark and then rebuild this repo, adding the `-DBUILD_BENCHMARK=ON` cmake flag. This will build a `/usr/bin/Benchmark` app that can be run

```
2022-07-28T12:01:23+01:00
Running ./Benchmark
Run on (8 X 1896.43 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 16384 KiB (x1)
Load Average: 0.40, 0.43, 0.22
------------------------------------------------------------------
Benchmark                        Time             CPU   Iterations
------------------------------------------------------------------
ComRpcFixture/100Bytes       0.399 ms        0.076 ms         8959
ComRpcFixture/1KB            0.409 ms        0.076 ms         8746
ComRpcFixture/10KB           0.438 ms        0.083 ms         8677
ComRpcFixture/100KB          0.576 ms        0.093 ms         7651
ComRpcFixture/1MB            0.820 ms        0.162 ms         4391
JsonRpcFixture/100Bytes      0.440 ms        0.081 ms         8707
JsonRpcFixture/1KB            3.80 ms        0.282 ms         1000
JsonRpcFixture/10KB           15.6 ms         1.60 ms          462
JsonRpcFixture/100KB          53.1 ms         13.3 ms           60
JsonRpcFixture/1MB             366 ms         82.0 ms            8
```