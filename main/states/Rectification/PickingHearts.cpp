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

#include "PickingHearts.h"
#include <string>
#include <cmath>
#include "InitialStabilization.h"

#include "../../common.h"
#include "../../hw/DisplayHelper.h"
#include "Buzzer.h"
#include "../Greeting.h"
#include "StabilizingHearts.h"
#include "PickingHeads.h"

void PickingHearts::entry() {
    xQueueReset(Distiller::eventQueue);

    lastTresholdExceedTime = heartsStartTime;
    DisplayHelper::Clear();
    DisplayHelper::Print(0, 8, "   Picking HEARTS   ");

    headsValve.Close();
    heartsValve.Open();

    start_timer();
}

void PickingHearts::exit() {
    stop_timer();
    headsValve.Close();
    heartsValve.Close();
}

void PickingHearts::react(const TimerTickEvent &event) {
    auto temp = columnSensor.temp();
    auto cubeTemp = tankSensor.temp();
    auto defTemp = defSensor.temp();

    if (temp == TEMP_UNDEFINED) {
        // TODO: create state "WaitingForSensor"
        DisplayHelper::Clear();
        DisplayHelper::Print(0, 8, "Waiting for sensor");
        return;
    }

    if (heartsStartTemp == TEMP_UNDEFINED) {
        heartsStartTemp = temp;
        ESP_LOGI(TAG, "Hearts start temp: %.2f C\n", heartsStartTemp);
    }

    DisplayHelper::Print(0, 16, "Cube   Column Dephl.");
    DisplayHelper::PrintFmt(0, 24, "%#05.2f. %#05.2f. %#05.2f.", cubeTemp, temp, defTemp);

    time_t nowTime = TimeHelper::GetTimestamp();

    time_t timePassed = nowTime - lastTresholdExceedTime;
    time_t totalTime = nowTime - heartsStartTime;
    auto timeinfoPassed = TimeHelper::GetTimeInfo(timePassed);
    auto timeinfoTotal = TimeHelper::GetTimeInfo(totalTime);

    DisplayHelper::PrintFmt(0, 32, "Stable: %02u:%02u:%02u ", timeinfoPassed.tm_hour, timeinfoPassed.tm_min,
                            timeinfoPassed.tm_sec);
    DisplayHelper::PrintFmt(0, 40, "Total : %02u:%02u:%02u ", timeinfoTotal.tm_hour, timeinfoTotal.tm_min,
                            timeinfoTotal.tm_sec);

    DisplayHelper::Print(0, 48, "Hearts valve: ");
    DisplayHelper::Print(6 * 14, 48, heartsValve.IsOpen() ? "OPEN  " : "CLOSED");
    DisplayHelper::PrintFmt(0, 56, "Goal temp: %.2f", heartsStartTemp);

    auto delta = temp - heartsStartTemp;

    if (delta > HEARTS_STABILIZE_DELTA) {
        lastTresholdExceedTime = nowTime;

        send_event(new HeartsDistillingTempExceededEvent());
    }
}

void PickingHearts::react(const HeartsDistillingTempExceededEvent &event) {
    transit<StabilizingHearts>();
}

void PickingHearts::react(const ButtonClickDistillerEvent &event) {
    buzzer.beep(Buzzer::NOTE_B5, 25);

    switch (event.button()) {
        case ENTER:
            transit<MainMenu>();
            break;
        case EXIT:
            transit<PickingHeads>();
            break;
        case UP:
            heartsValve.Close();
            break;
        case DOWN:
            heartsValve.Open();
            break;
        case LEFT:
            lastTresholdExceedTime = TimeHelper::GetTimestamp();
            heartsStartTemp = (roundf(heartsStartTemp * 100) / 100.0f) - 0.01f;
            break;
        case RIGHT:
            lastTresholdExceedTime = TimeHelper::GetTimestamp();
            heartsStartTemp = (roundf(heartsStartTemp * 100) / 100.0f) + 0.01f;
            break;
        default:
            break;
    }
}

