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

#include "PickingHeads.h"
#include <string>
#include "../../hw/DisplayHelper.h"
#include "../Greeting.h"
#include "PickingHearts.h"
#include "Buzzer.h"

void PickingHeads::entry() {
    xQueueReset(Distiller::eventQueue);

    initialTime = TimeHelper::GetTimestamp();
    prevTime = initialTime;
    lastTresholdExceedTime = initialTime;
    prevTemp = TEMP_UNDEFINED;

    heartsValve.Close();
    headsValve.Open();

    DisplayHelper::Clear();
    DisplayHelper::Print(0, 8, "   Picking HEADS    ");
    start_timer();
}

void PickingHeads::exit() {
    stop_timer();
    headsValve.Close();
    heartsValve.Close();
}

void PickingHeads::react(const TimerTickEvent &event) {
    auto temp = columnSensor.temp();
    auto cubeTemp = tankSensor.temp();
    auto defTemp = defSensor.temp();

    if (temp == TEMP_UNDEFINED) {
        // TODO: create state "WaitingForSensor"
        DisplayHelper::Clear();
        DisplayHelper::Print(0, 8, "Waiting for sensor");
        return;
    }

    DisplayHelper::Print(0, 16, "Cube   Column Dephl.");
    DisplayHelper::PrintFmt(0, 24, "%#05.2f. %#05.2f. %#05.2f.", cubeTemp, temp, defTemp);

    time_t nowTime = TimeHelper::GetTimestamp();

    time_t totalTime = nowTime - initialTime;
    auto timeinfoTotal = TimeHelper::GetTimeInfo(totalTime);

    DisplayHelper::PrintFmt(0, 32, "Time: %02u:%02u:%02u ", timeinfoTotal.tm_hour, timeinfoTotal.tm_min,
                            timeinfoTotal.tm_sec);

    DisplayHelper::Print(0, 40, "Heads valve: ");
    DisplayHelper::Print(6 * 13, 40, headsValve.IsOpen() ? "OPEN" : "CLOSED");

    if (nowTime - prevTime > MINUTES(30)) {
        buzzer.beep_attention();
        prevTime = nowTime;
    }
}

void PickingHeads::react(const ButtonClickDistillerEvent &event) {
    buzzer.beep_btn();

    switch (event.button()) {
        case ENTER:
            heartsStartTime = TimeHelper::GetTimestamp();
            heartsStartTemp = TEMP_UNDEFINED;
            transit<PickingHearts>();
            break;
        case EXIT:
            transit<MainMenu>();
            break;
        case UP:
            headsValve.Close();
            break;
        case DOWN:
            headsValve.Open();
            break;
        default:
            break;
    }
}
