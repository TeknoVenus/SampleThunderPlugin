#pragma once

#include "Module.h"

#include <functional>
#include <random>

namespace WPEFramework
{
    namespace Plugin
    {
        using CallbackFn = std::function<void(uint64_t)>;

        /**
         * For the example, we want to have a regularly scheduled timer
         * that can trigger a callback (mostly so we can demo notifications
         * over both JSON and COM-RPC)
         *
         * Implement a simple class that can be triggered by Thunder's Timer implementation
         */
        class SamplePluginTimer
        {
        public:
            SamplePluginTimer() = delete;
            ~SamplePluginTimer() = default;

            /**
             * For the sake of the demo, we expect a unique ID per instance
             * of this timer handler
             */
            SamplePluginTimer(CallbackFn callback,
                              uint32_t timerId)
                : _callback(callback),
                  _timerId(timerId)
            {
            }

            SamplePluginTimer(const SamplePluginTimer &copy)
                : _callback(copy._callback),
                  _timerId(copy._timerId)
            {
            }

            bool operator==(const SamplePluginTimer &RHS) const
            {
                return (_timerId == RHS._timerId);
            }

        public:
            uint64_t Timed(const uint64_t scheduledTime)
            {
                SYSLOG(Logging::Startup, (_T("Timer event triggered! Firing callback")));
                _callback(scheduledTime);
                return 0;
            }

        private:
            CallbackFn _callback;
            const uint32_t _timerId;
        };
    }
}