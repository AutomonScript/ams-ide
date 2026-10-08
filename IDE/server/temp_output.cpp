#include <iostream>
#include <string>
#include <functional>
#include <cmath>
#include <thread>
#include <chrono>
#include <map>
#include "include/stdlib/io/console.hpp"
#include "include/runtime/runtime.hpp"
#include "include/stdlib/source/log_source.hpp"
#include "include/stdlib/source/log_data.hpp"
#include "include/stdlib/source/log_record.hpp"
#include "include/stdlib/source/filter_criteria.hpp"

using namespace ams::stdlib::source;

int main() {
    ams::Runtime rt;
    auto& trackCopyManager = rt.getTrackCopyManager();
    auto& eventBus = rt.getEventBus();
    auto& scheduler = rt.getScheduler();

    rt.init();

        bool trigger = true;
        int X = 11;
FUNC_PRINT("Hello! ");
FUNC_PRINTLN("WORLD");
    // EVENT: HELLO_E ON HELLO_S
    eventBus.subscribe("HELLO_E", "HELLO_E_handler", [&](const ams::EventPayload& __payload) {
        // Get SignalContext from payload
        auto __signal_context = trackCopyManager.getContext(__payload.contextId);
        if (!__signal_context) return;

        // SIGNAL statement in EVENT
        {
            bool __signal_condition = true;
            if (__signal_condition) {
                std::map<std::string, ams::Snapshot::Value> __event_vars;
                std::string __new_context_id = trackCopyManager.createEventContext(
                    "HELLO_E",
                    __event_vars,
                    __payload.contextId
                );
                trackCopyManager.addReference(__new_context_id);
                eventBus.publish("HELLO_O", {"HELLO_E", "HELLO_O", 0, __new_context_id});
                eventBus.publish("HELLO_O2", {"HELLO_E", "HELLO_O2", 0, __new_context_id});
            }
        }
FUNC_PRINTLN("Hello! ", "From EVENT");
        trackCopyManager.releaseContext(__payload.contextId);
    });

    // OBSERVER: HELLO_O OBSERVS HELLO_E
    eventBus.subscribe("HELLO_O", "HELLO_O_handler", [&](const ams::EventPayload& __payload) {
        // Get SignalContext from payload
        auto __signal_context = trackCopyManager.getContext(__payload.contextId);
        if (!__signal_context) return;

FUNC_PRINTLN("Hello! ", "From OBSERVER ", "x 1");
        trackCopyManager.releaseContext(__payload.contextId);
    });

    // OBSERVER: HELLO_O2 OBSERVS HELLO_E
    eventBus.subscribe("HELLO_O2", "HELLO_O2_handler", [&](const ams::EventPayload& __payload) {
        // Get SignalContext from payload
        auto __signal_context = trackCopyManager.getContext(__payload.contextId);
        if (!__signal_context) return;

FUNC_PRINTLN("Hello! ", "From OBSERVER ", "x 2");
        trackCopyManager.releaseContext(__payload.contextId);
    });

    // SOURCE: HELLO_S
    scheduler.schedulePeriodic("HELLO_S", 0, [&]() {
        trigger = (!trigger);
FUNC_PRINTLN("Hello! ", "From SOURCE");
        // SIGNAL statement
        {
            bool __signal_condition = (X > 10);
            if (__signal_condition) {
                std::map<std::string, ams::Snapshot::Value> __track_vars;
                std::string __context_id = trackCopyManager.createTrackCopy(
                    "HELLO_S",
                    __track_vars
                );
                eventBus.publish("HELLO_E", {"HELLO_S", "HELLO_E", 0, __context_id});
            }
        }
    });


    // --- Execution Logic ---

    while(true) { std::this_thread::sleep_for(std::chrono::seconds(1)); }
    rt.shutdown();
    return 0;
}
