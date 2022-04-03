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

#ifndef MAGIC_DISTILLER_DISTILLER_H
#define MAGIC_DISTILLER_DISTILLER_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <freertos/timers.h>
#include "tinyfsm.hpp"
#include "../events/DistillerEvent.h"
#include "../common.h"
#include "../hw/Valve.h"
#include "Buzzer.h"
#include "../hw/TempSensor.h"

class Distiller : public tinyfsm::Fsm<Distiller> {
private:
    static int volatile timerCount;

    class Initializer {
    public:
        Initializer();
    };

    friend class Initializer;

    static Initializer initializer;

protected:
    static const char *TAG;
    static TimerHandle_t timer;

    static Valve headsValve;
    static Valve heartsValve;
    static time_t heartsStartTime;
    static float heartsStartTemp;

public:
    static TempSensor defSensor;
    static TempSensor columnSensor;
    static TempSensor tankSensor;
    static const QueueHandle_t eventQueue;
    static Buzzer buzzer;

    /* default reaction for unhandled events */
    void react(tinyfsm::Event const &) {};

    virtual void entry(void) {};
    virtual void exit(void) {};

    static void send_event(DistillerEvent *event) {
        if (xQueueSend(Distiller::eventQueue, &event, 5 / portTICK_PERIOD_MS) != pdTRUE) {
            ESP_LOGE(TAG, "Can't send event %d, queue is full", event->_type);
            delete event;
        }
    };

    void start_timer(uint32_t period = 100) {
        timerCount++;
        ESP_LOGI(TAG, "Starting timer, count %d", timerCount);

        xTimerChangePeriod(Distiller::timer, pdMS_TO_TICKS(period), pdMS_TO_TICKS(10));
        xTimerStart(Distiller::timer, pdMS_TO_TICKS(10));
    }

    virtual void stop_timer() {
        ESP_LOGI(TAG, "Stopping timer, count %d", timerCount);
        timerCount--;

        xTimerStop(timer, pdMS_TO_TICKS(10));
    }

    virtual void react(ButtonClickDistillerEvent const &) {};

    virtual void react(TimerTickEvent const &) {};

    virtual void react(const InitialStabilizationDoneEvent &) {};

    virtual void react(const InitialStabilizationImpossibleEvent &) {};

    virtual void react(const HeadsDistillingDoneEvent &) {};

    virtual void react(const HeartsDistillingDoneEvent &) {};

    virtual void react(const HeartsDistillingTempExceededEvent &) {};

    virtual void react(const HeartsDistillingTempStabilizedEvent &) {};

    virtual void react(const HeartsDistillingUnableToStabilizeTempEvent &) {};

};

#endif //MAGIC_DISTILLER_DISTILLER_H
