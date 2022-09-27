# Sample Thunder Plugin
A simple Thunder plugin for experimenting and learning about Thunder. **This is designed for use with Thunder R3/4, it will not build on R2 (version currently used in RDK) any more**

Has the following features
* JSON-RPC API
* COM-RPC API
* Can run in-process and out-of-process
* Contains test applications for COM and JSON-RPC

# Build
Set up a build environment that can build Thunder. You can do this manually following the steps below, or use [Thunder KickStart](https://github.com/WebPlatformForEmbedded/ThunderKickStart/)

This requires cloning the following Thunder repos:

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

* Copy the `Interfaces/ISamplePlugin.h` interface from this repo to `ThunderInterfaces/interfaces`. This is the COM-RPC and JSON-RPC API we implement
* Add a unique ID value for `ID_SAMPLE_PLUGIN` and `ID_SAMPLE_PLUGIN_NOTIFICATION` to `IDs.h` in ThunderInterfaces

Now build Thunder (only need to do this once). In all below instructions, run the commands in the root directory you cloned all the repos into

```shell
mkdir ./build

# Thunder Tools
cmake -HThunder/Tools -Bbuild/ThunderTools -DCMAKE_INSTALL_PREFIX=/usr
make -j4 -C build/ThunderTools && sudo make -C build/ThunderTools install

# Thunder Core
cmake -HThunder -Bbuild/Thunder -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_TYPE=Debug -DBINDING=127.0.0.1 -DPORT=9998
make -j4 -C build/Thunder && sudo make -C build/Thunder install
```

Now build Thunder interfaces (do this every time you change `ISamplePlugin.h`)
```shell
cmake -HThunderInterfaces -Bbuild/ThunderInterfaces  -DCMAKE_INSTALL_PREFIX=/usr
make -j4 -C build/ThunderInterfaces && sudo make -C build/ThunderInterfaces install
```

Finally build this repo containing the sample plugin & accompanying client libraries and test apps
```
cmake -HSamplethunderplugin -Bbuild/Samplethunderplugin -DCMAKE_INSTALL_PREFIX=/usr
make -j4 -C build/Samplethunderplugin && sudo make -C build/Samplethunderplugin install
```

# Usage
First, start WPEFramework by running `/usr/bin/WPEFramework`. You should see SamplePlugin autostart.

Press `q` to terminate WPEFramework.

## Enable SamplePlugin tracing
By default, WPEFramework does not enable tracing for plugins. Enable all trace levels for SamplePlugin by setting the trace state for `Plugin_SamplePlugin` to enabled.

The mechanism for this varies slightly if you are using the older Tracing module in Thunder, or the new Messaging system on R4.

Note that tracing won't appear if you don't have the TraceControl or MessagingControl plugin also installed and activated in Thunder.

## JSON-RPC
Make the following JSON-RPC request to the sample plugin to generate a greeting

```
curl --request POST \
  --url http://127.0.0.1:55555/jsonrpc \
  --header 'Content-Type: application/json' \
  --data '{
	"jsonrpc": "2.0",
	"id": 4,
	"method": "SamplePlugin.greet",
	"params": {
		"message": "World"
	}
}'
```
The plugin should respond with a generated greeting
```json
{
	"jsonrpc": "2.0",
	"id": 4,
	"result": "Good Morning, World"
}
```

If there is an error, it will be returned as per the JSON-RPC spec

```json
{
	"jsonrpc": "2.0",
	"id": 4,
	"error": {
		"code": 30,
		"message": "ERROR_BAD_REQUEST"
	}
}
```

You can also use the JSONRPC test app included in the repo to see how to make requests from C++ code.

## COM-RPC
To test COM-RPC, use the test client. For test purposes, the test client will activate & deactivate the plugin when it starts/exits respectively.

## Switch between In-Process and Out-Of-Process execution
To change if the plugin runs in or out-of-process, edit the `SamplePlugin.config` file in `Samplethunderplugin/SamplePlugin/SamplePlugin.config` and rebuild. Then restart WPEFramework.

When running out-of-process, you'll see a new WPEProcess spawned when the plugin is activated
```
vagrant    36077  0.0  0.5 618988 21904 pts/1    Sl+  12:00   0:00  |           \_ WPEFramework
vagrant    37676  0.0  0.3  95684 14360 pts/1    Sl+  12:07   0:00  |               \_ WPEProcess -l libWPEFrameworkSamplePlugin.so -c SamplePlugin -C SamplePlugin -r /tmp/communicator -i 224 -x 5 -p "/root/SamplePlugin/" -s "/usr/lib/wpeframework/plugins/" -d "/usr/share/WPEFramework/SamplePlugin/" -a "/usr/bin/" -v "/tmp/SamplePlugin/" -m "/usr/lib/wpeframework/proxystubs/" -P "/opt/minidumps/" -e 127
```

The sampleplugin logging will print the PID of the process it's running in to easily verify if everything is working as expected.

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