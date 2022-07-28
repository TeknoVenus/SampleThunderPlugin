/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2022 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/

#include <benchmark/benchmark.h>

#include "comrpc.h"
#include "jsonrpc.h"


// COM-RPC

BENCHMARK_DEFINE_F(ComRpcFixture, 100Bytes)
(benchmark::State &st)
{
    for (auto _ : st)
    {
        Echo100B(st);
    }
}
BENCHMARK_REGISTER_F(ComRpcFixture, 100Bytes)->Unit(benchmark::kMillisecond);

BENCHMARK_DEFINE_F(ComRpcFixture, 1KB)
(benchmark::State &st)
{
    for (auto _ : st)
    {
        Echo1K(st);
    }
}
BENCHMARK_REGISTER_F(ComRpcFixture, 1KB)->Unit(benchmark::kMillisecond);

BENCHMARK_DEFINE_F(ComRpcFixture, 10KB)
(benchmark::State &st)
{
    for (auto _ : st)
    {
        Echo10K(st);
    }

}
BENCHMARK_REGISTER_F(ComRpcFixture, 10KB)->Unit(benchmark::kMillisecond);

BENCHMARK_DEFINE_F(ComRpcFixture, 100KB)
(benchmark::State &st)
{
    for (auto _ : st)
    {
        Echo100K(st);
    }
}
BENCHMARK_REGISTER_F(ComRpcFixture, 100KB)->Unit(benchmark::kMillisecond);

BENCHMARK_DEFINE_F(ComRpcFixture, 1MB)
(benchmark::State &st)
{
    for (auto _ : st)
    {
        Echo1M(st);
    }
}
BENCHMARK_REGISTER_F(ComRpcFixture, 1MB)->Unit(benchmark::kMillisecond);


// JSON-RPC

BENCHMARK_DEFINE_F(JsonRpcFixture, 100Bytes)
(benchmark::State &st)
{
    for (auto _ : st)
    {
        Echo100B(st);
    }
}
BENCHMARK_REGISTER_F(JsonRpcFixture, 100Bytes)->Unit(benchmark::kMillisecond);

BENCHMARK_DEFINE_F(JsonRpcFixture, 1KB)
(benchmark::State &st)
{
    for (auto _ : st)
    {
        Echo1K(st);
    }
}
BENCHMARK_REGISTER_F(JsonRpcFixture, 1KB)->Unit(benchmark::kMillisecond);

BENCHMARK_DEFINE_F(JsonRpcFixture, 10KB)
(benchmark::State &st)
{
    for (auto _ : st)
    {
        Echo10K(st);
    }
}
BENCHMARK_REGISTER_F(JsonRpcFixture, 10KB)->Unit(benchmark::kMillisecond);

BENCHMARK_DEFINE_F(JsonRpcFixture, 100KB)
(benchmark::State &st)
{
    for (auto _ : st)
    {
        Echo100K(st);
    }
}
BENCHMARK_REGISTER_F(JsonRpcFixture, 100KB)->Unit(benchmark::kMillisecond);

BENCHMARK_DEFINE_F(JsonRpcFixture, 1MB)
(benchmark::State &st)
{
    for (auto _ : st)
    {
        Echo1M(st);
    }
}
BENCHMARK_REGISTER_F(JsonRpcFixture, 1MB)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();