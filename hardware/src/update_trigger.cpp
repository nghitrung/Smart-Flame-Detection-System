#include "update_trigger.h"

bool updateTrigger(bool trigger, BlinkState& state, uint32_t onMs, uint32_t offMs) {
    const uint32_t nowMs = millis();
    
    if (!trigger) {
        state.wasTriggered = false;
        state.outputState = false;
        return false;
    }

    if (!state.wasTriggered) {
        // Start with a full ON pulse exactly at trigger time.
        state.wasTriggered = true;
        state.outputState = true;
        state.lastTransitionMs = nowMs;
        return true;
    }

    uint32_t interval = state.outputState ? onMs : offMs;
    if (nowMs - state.lastTransitionMs >= interval) {
        state.outputState = !state.outputState;
        state.lastTransitionMs = nowMs;
    }

    return state.outputState;
}
