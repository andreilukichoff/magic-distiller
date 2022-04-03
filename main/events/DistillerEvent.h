/*
 * The MIT License
 *
 * Copyright (c) 2022 Andrei Lukichev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MAGIC_DISTILLER_DISTILLEREVENT_H
#define MAGIC_DISTILLER_DISTILLEREVENT_H

#include <cstdint>
#include "tinyfsm.hpp"
#include "../common.h"

enum DistillerEventType {
    Unknown = 0,
    StartTimer,
    StopTimer,
    TimerTick,
    ButtonClick,
    InitialStabilizationImpossible,
    InitialStabilizationDone,
    HeadsDistillingDone,
    HeartsDistillingDone,
    HeartsDistillingTempExceeded,
    HeartsDistillingTempStabilized,
    HeartsDistillingUnableToStabilizeTemp
};

enum Button {
    UNKNOWN = 0,
    ENTER = 10,
    EXIT = 20,
    LEFT = 30,
    RIGHT = 40,
    UP = 50,
    DOWN = 60
};

struct DistillerEvent : tinyfsm::Event {
    DistillerEvent(DistillerEventType const type = DistillerEventType::Unknown) : _type{type} {}
    DistillerEventType _type {};
};

struct TimerTickEvent: DistillerEvent {
    TimerTickEvent() : DistillerEvent(DistillerEventType::TimerTick) {};
};

struct StopTimerEvent: DistillerEvent {
    StopTimerEvent() : DistillerEvent(DistillerEventType::StopTimer) {};
};

struct InitialStabilizationImpossibleEvent: DistillerEvent {
    InitialStabilizationImpossibleEvent() : DistillerEvent(DistillerEventType::InitialStabilizationImpossible) {};
};

struct InitialStabilizationDoneEvent: DistillerEvent {
    InitialStabilizationDoneEvent() : DistillerEvent(DistillerEventType::InitialStabilizationDone) {};
};

struct HeadsDistillingDoneEvent: DistillerEvent {
    HeadsDistillingDoneEvent() : DistillerEvent(DistillerEventType::HeadsDistillingDone) {};
};

struct HeartsDistillingDoneEvent: DistillerEvent {
    HeartsDistillingDoneEvent() : DistillerEvent(DistillerEventType::HeartsDistillingDone) {};
};

struct HeartsDistillingTempExceededEvent: DistillerEvent {
    HeartsDistillingTempExceededEvent() : DistillerEvent(DistillerEventType::HeartsDistillingTempExceeded) {};
};

struct HeartsDistillingTempStabilizedEvent: DistillerEvent {
    HeartsDistillingTempStabilizedEvent() : DistillerEvent(DistillerEventType::HeartsDistillingTempStabilized) {};
};

struct HeartsDistillingUnableToStabilizeTempEvent: DistillerEvent {
    HeartsDistillingUnableToStabilizeTempEvent() : DistillerEvent(DistillerEventType::HeartsDistillingUnableToStabilizeTemp) {};
};

struct ButtonClickDistillerEvent : DistillerEvent {
public:
    Button t_;
    explicit ButtonClickDistillerEvent(Button t) : DistillerEvent(DistillerEventType::ButtonClick) {
        t_ = t;
    }
    Button button() const {
        return t_;
    }
};

struct StartTimerEvent: DistillerEvent {
public:
    uint32_t period_;
    explicit StartTimerEvent(uint32_t period) : DistillerEvent(DistillerEventType::StartTimer) {
        period_ = period;
    }
    uint32_t period() const {
        return period_;
    }
};

#endif //MAGIC_DISTILLER_DISTILLEREVENT_H
