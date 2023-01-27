#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "Module.h"

/**
 * Create our own version of the SmartLink class with extra functionality
 */
namespace WPEFramework {

class SamplePluginLink : public JSONRPC::SmartLinkType<Core::JSON::IElement> {

public:
    SamplePluginLink(const string& callsign, const TCHAR* clientId)
        : JSONRPC::SmartLinkType<Core::JSON::IElement>(callsign, clientId)
    {
    }
    ~SamplePluginLink() = default;
    SamplePluginLink(const SamplePluginLink&) = delete;
    SamplePluginLink& operator=(const SamplePluginLink&) = delete;

    /**
     * Fired when the state of the plugin has changed
     */
    void StateChange() override
    {
        _establishedCondition.notify_one();
    }

    /**
     * @brief This method will block and wait for the JSON-RPC link to be
     * established with Thunder
     *
     * By default, the JSON-RPC connection is established asynchronously
     * when the SmartLink is constructed. This means we have no way to know
     * when we've established a connection to Thunder.
     *
     * Once a connection is established, the StateChange() method will be fired
     * to indicate that we now know the state of the plugin
     */
    bool WaitForLinkEstablish(int timeoutMs)
    {
        std::unique_lock<std::mutex> lock(_lock);
        auto result = _establishedCondition.wait_for(lock, std::chrono::milliseconds(timeoutMs));
        return result != std::cv_status::timeout;
    }

private:
    std::mutex _lock;
    std::condition_variable _establishedCondition;
};
}