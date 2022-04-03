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

#include "Distiller.h"

const char *Distiller::TAG = "distiller";
const QueueHandle_t Distiller::eventQueue = xQueueCreate(10, sizeof(void *));
Buzzer Distiller::buzzer = Buzzer();
Valve Distiller::headsValve = Valve(VALVE_HEADS_GPIO);
Valve Distiller::heartsValve = Valve(VALVE_HEARTS_GPIO);
time_t Distiller::heartsStartTime;
float Distiller::heartsStartTemp;
volatile int Distiller::timerCount = 0;
TimerHandle_t Distiller::timer;
TempSensor Distiller::defSensor = TempSensor(SENSOR_DEF_GPIO);
TempSensor Distiller::columnSensor = TempSensor(SENSOR_COL_GPIO);
TempSensor Distiller::tankSensor = TempSensor(SENSOR_TANK_GPIO);
Distiller::Initializer Distiller::initializer;

Distiller::Initializer::Initializer() {
    timer = xTimerCreate("timer", pdMS_TO_TICKS(250), pdTRUE, (void *) 0,
                         [](TimerHandle_t xTimer) {
                             configASSERT(xTimer);
                             auto event2 = new TimerTickEvent();
                             Distiller::send_event(event2);
                         });
}

