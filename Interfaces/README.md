# Interfaces
The COM-RPC and JSON-RPC APIs implemented by the sample plugin

Copy these to ThunderInterfaces repo.

* Copy the `Interfaces/ISamplePlugin.h` interface to `ThunderInterfaces/interfaces`. This is the COM-RPC API we implement
* Copy the `Interfaces/SamplePlugin.json` schema to `ThunderInterfaces/jsonrpc`. This is the JSON-RPC API we implement
* Add a unique ID value for `ID_SAMPLE_PLUGIN` and `ID_SAMPLE_PLUGIN_NOTIFICATION` to `IDs.h` in ThunderInterfaces