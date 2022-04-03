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

#ifndef MAGIC_DISTILLER_RECTIFIER_H
#define MAGIC_DISTILLER_RECTIFIER_H

#include "../Distiller.h"
#include "../../hw/TimeHelper.h"

class Rectifier : public Distiller {
protected:
    static float heartsStabilizeDelta;
    static float initialStabilizeDelta;
    static float delta, currentTemp, prevTemp;
    static time_t initialTime, lastTresholdExceedTime, nowTime, timePassed, timeTotal;

public:
    static void reset() {
        delta = 0;
        currentTemp = 0;
        prevTemp = 0;
        heartsStabilizeDelta = HEARTS_STABILIZE_DELTA;
        initialStabilizeDelta = INITIAL_STABILIZE_DELTA;
        initialTime = TimeHelper::GetTimestamp();
        lastTresholdExceedTime = initialTime;
        prevTemp = TEMP_UNDEFINED;
        update_vars();
    }

    static void update_vars() {
        currentTemp = columnSensor.temp();
        delta = abs(currentTemp - prevTemp);
        nowTime = TimeHelper::GetTimestamp();

        timePassed = nowTime - lastTresholdExceedTime;
        timeTotal = nowTime - initialTime;
    }
};

#endif //MAGIC_DISTILLER_RECTIFIER_H
