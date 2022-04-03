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

#include <string>
#include <cmath>
#include "InitialStabilization.h"

#include "../../common.h"
#include "../../hw/DisplayHelper.h"
#include "Buzzer.h"
#include "../Greeting.h"
#include "StabilizingHearts.h"
#include "PickingHearts.h"
#include "PickingHeads.h"

void StabilizingHearts::entry() {
    xQueueReset(Distiller::eventQueue);

    heartsValve.Close();
    headsValve.Close();

    buzzer.beep_bad();
    DisplayHelper::Clear();
    DisplayHelper::Print(0, 8, "   Picking HEARTS   ");

    ESP_LOGI(TAG, "heartsStartTemp: %.2f C", heartsStartTemp);

    initialTime = TimeHelper::GetTimestamp();

    start_timer();
}

void StabilizingHearts::exit() {
    stop_timer();
        heartsValve.Close();
    headsValve.Close();
}

void StabilizingHearts::react(const TimerTickEvent &event) {
    auto temp = columnSensor.temp();

    if (temp == TEMP_UNDEFINED) {
        // TODO: create state "WaitingForSensor"
        DisplayHelper::Clear();
        DisplayHelper::Print(0, 8, "Waiting for sensor");
        return;
    }

    auto nowTime = TimeHelper::GetTimestamp();
    auto timePassed = nowTime - lastTresholdExceedTime;
    auto timeLeft = HEARTS_STABILIZE_TIMEOUT - (nowTime - initialTime);

    auto delta = temp - heartsStartTemp;

    auto timeLeftTimeInfo = TimeHelper::GetTimeInfo(timeLeft);
    auto timeinfoPassed = TimeHelper::GetTimeInfo(timePassed);
    DisplayHelper::PrintFmt(0, 16, "Delta: %.2f   ", delta);
    DisplayHelper::PrintFmt(0, 24, "Temp: %#05.2f C", temp);

    if (delta > HEARTS_STABILIZE_DELTA) {
        if (nowTime - initialTime > HEARTS_STABILIZE_TIMEOUT) {
            ESP_LOGI(TAG, "Unable to stabilize");
            ESP_LOGI(TAG, "heartsStartTemp: %.2f C, current temp: %.2f C, delta: %.2f C", heartsStartTemp, temp, delta);
            send_event(new HeartsDistillingUnableToStabilizeTempEvent());
        }
        if (delta > 0.25) {
            ESP_LOGI(TAG, "delta > 0.25, distillation finished");
            send_event(new HeartsDistillingUnableToStabilizeTempEvent());
        }

        lastTresholdExceedTime = nowTime;

        DisplayHelper::PrintFmt(0, 32, "Goal: %.2f          ", heartsStartTemp);
        DisplayHelper::PrintFmt(0, 40, "Time left: %02u:%02u:%02u", timeLeftTimeInfo.tm_hour, timeLeftTimeInfo.tm_min,
                                timeLeftTimeInfo.tm_sec);
        DisplayHelper::Print(0, 56, "   STABILIZING...   ");
    } else {
        auto timeLeft = HEARTS_STABILIZE_CONFIRM - timePassed;
        auto timeLeftTimeInfo = TimeHelper::GetTimeInfo(timeLeft);

        DisplayHelper::PrintFmt(0, 32, "Stable: %02u:%02u:%02u ", timeinfoPassed.tm_hour, timeinfoPassed.tm_min,
                                timeinfoPassed.tm_sec);
        DisplayHelper::PrintFmt(0, 40, "Time left: %02u:%02u:%02u", timeLeftTimeInfo.tm_hour, timeLeftTimeInfo.tm_min,
                                timeLeftTimeInfo.tm_sec);

        DisplayHelper::ClearRow(56);

        if (timePassed > HEARTS_STABILIZE_CONFIRM) {
            ESP_LOGI(TAG, "Stabilized");
            send_event(new HeartsDistillingTempStabilizedEvent());
        }
    }
}

void StabilizingHearts::react(const HeartsDistillingTempStabilizedEvent &event) {
    transit<PickingHearts>();
}

void StabilizingHearts::react(const HeartsDistillingUnableToStabilizeTempEvent &event) {
    stop_timer();
    heartsValve.Close();
    headsValve.Close();
    DisplayHelper::Clear();
    DisplayHelper::Print(0, 0, "DONE");
    buzzer.beep_good();
    // transit<DistillationDone>();
}

void StabilizingHearts::react(const ButtonClickDistillerEvent &event) {
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
