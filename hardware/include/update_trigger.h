#ifndef UPDATE_TRIGGER_H
#define UPDATE_TRIGGER_H

#include "global.h"

struct BlinkState {
    bool wasTriggered = false;
    bool outputState = false;
    uint32_t lastTransitionMs = 0;
};

bool updateTrigger(bool trigger, BlinkState& state, uint32_t onMs, uint32_t offMs);

#endif