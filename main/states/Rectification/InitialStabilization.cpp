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

#include <cmath>
#include "InitialStabilization.h"

#include "../../common.h"
#include "../../hw/DisplayHelper.h"
#include "Buzzer.h"
#include "../Greeting.h"
#include "PickingHeads.h"

void InitialStabilization::entry() {
    xQueueReset(Distiller::eventQueue);

    DisplayHelper::Clear();
    DisplayHelper::Animate(0, 8, "Stabilizing...");

    reset();
    start_timer();
}

void InitialStabilization::exit() {
    stop_timer();
}

void InitialStabilization::update_display() {
    auto tmPassed = TimeHelper::GetTimeInfo(timePassed);
    auto tmTotal = TimeHelper::GetTimeInfo(timeTotal);

    DisplayHelper::PrintFmt(0, 24, "temp: %#05.2f", currentTemp);
    DisplayHelper::PrintFmt(0, 32, "d: %.2f set: %.2f", delta, initialStabilizeDelta);
    DisplayHelper::PrintFmt(0, 40, "Stable: %02u:%02u:%02u ", tmPassed.tm_hour, tmPassed.tm_min,
                            tmPassed.tm_sec);
    DisplayHelper::PrintFmt(0, 48, "Total : %02u:%02u:%02u ", tmTotal.tm_hour, tmTotal.tm_min,
                            tmTotal.tm_sec);
}

void InitialStabilization::react(const TimerTickEvent &event) {
    update_vars();
    update_display();

    if (currentTemp == TEMP_UNDEFINED) {
        DisplayHelper::Clear();
        DisplayHelper::Print(0, 8, "Waiting for sensor");
        return;
    }

    // if temperature delta > 0.1 then column is unstable, reset time
    if (delta > initialStabilizeDelta) {
        buzzer.beep_bad();
        lastTresholdExceedTime = nowTime;
        ESP_LOGI(TAG, "Temp threshold exceeded (%.2f C), resetting time...\n", delta);
        prevTemp = currentTemp;
    } else {
        // if temperature delta < 0.1 for more than 5 minutes then column is stable
        if (timePassed > INITIAL_STABILIZE_TIME) {
            send_event(new InitialStabilizationDoneEvent());
        }
    }

    if (nowTime - initialTime > INITIAL_STABILIZE_TIMEOUT) {
        send_event(new InitialStabilizationImpossibleEvent());
    }
}

void InitialStabilization::react(const InitialStabilizationDoneEvent &event) {
    stop_timer();

    DisplayHelper::Clear();
    DisplayHelper::Print(0, 48, "Column stabilized.");
    buzzer.beep_good();

    transit<PickingHeads>();
}

void InitialStabilization::react(const InitialStabilizationImpossibleEvent &event) {
    stop_timer();

    DisplayHelper::Clear();
    DisplayHelper::Print(0, 16, "Unable to stabilize");
    buzzer.beep_toobad();
}

void InitialStabilization::react(const ButtonClickDistillerEvent &event) {
    buzzer.beep(Buzzer::NOTE_B5, 25);

    switch (event.button()) {
        case ENTER:
            transit<PickingHeads>();
            break;
        case EXIT:
            transit<MainMenu>();
            break;
        case UP:
            heartsValve.Toggle();
            break;
//        case LEFT:
//            heartsStartTemp = (roundf(treshold * 100.0f) / 100.0f) - 0.05f;
//            break;
//        case RIGHT:
//            heartsStartTemp = (roundf(treshold * 100.0f) / 100.0f) + 0.05f;
//            break;
        default:
            break;
    }
}