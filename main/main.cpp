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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>
#include <i2cdev.h>
#include <ds18x20.h>
#include <cstring>
#include <nvs_flash.h>
#include <freertos/timers.h>

#include "sdkconfig.h"
#include "common.h"
#include "hw/TimeHelper.h"
#include "tinyfsm.hpp"
#include "states/Distiller.h"
#include "events/DistillerEvent.h"
#include "button.h"
#include "Buzzer.h"
#include "ssd1306.h"
#include "buttons.h"
#include "states/Greeting.h"

static const char *TAG = "main";

FSM_INITIAL_STATE(Distiller, Greeting)

void dispatch_queue_event() {
    DistillerEvent *event_p = nullptr;
    if ((pdTRUE == xQueueReceive(Distiller::eventQueue, &event_p, pdMS_TO_TICKS(100)) && (event_p != nullptr))) {
        //ESP_LOGI(TAG, "dispatch event, type: %d", (int) event_p->_type);

        switch (event_p->_type) {
            case DistillerEventType::ButtonClick: {
                ESP_LOGI(TAG, "Received ButtonClick");
                auto event = reinterpret_cast<ButtonClickDistillerEvent *>(event_p);
                Distiller::dispatch(*event);
                break;
            }
            case DistillerEventType::TimerTick: {
                //sESP_LOGI(TAG, "Received TimerTick");
                auto event = reinterpret_cast<TimerTickEvent *>(event_p);
                Distiller::dispatch(*event);
                break;
            }
            case DistillerEventType::InitialStabilizationDone: {
                ESP_LOGI(TAG, "Received InitialStabilizationDone");
                auto event = reinterpret_cast<InitialStabilizationDoneEvent *>(event_p);
                Distiller::dispatch(*event);
                break;
            }
            case DistillerEventType::InitialStabilizationImpossible: {
                ESP_LOGI(TAG, "Received InitialStabilizationImpossible");
                auto event = reinterpret_cast<InitialStabilizationImpossibleEvent *>(event_p);
                Distiller::dispatch(*event);
                break;
            }
            case DistillerEventType::HeadsDistillingDone: {
                ESP_LOGI(TAG, "Received HeadsDistillingDone");
                auto event = reinterpret_cast<HeadsDistillingDoneEvent *>(event_p);
                Distiller::dispatch(*event);
                break;
            }
            case DistillerEventType::HeartsDistillingDone: {
                ESP_LOGI(TAG, "Received HeartsDistillingDone");
                auto event = reinterpret_cast<HeartsDistillingDoneEvent *>(event_p);
                Distiller::dispatch(*event);
                break;
            }
            case DistillerEventType::HeartsDistillingTempExceeded: {
                ESP_LOGI(TAG, "Received HeartsDistillingTempExceeded");
                auto event = reinterpret_cast<HeartsDistillingTempExceededEvent *>(event_p);
                Distiller::dispatch(*event);
                break;
            }
            case DistillerEventType::HeartsDistillingTempStabilized: {
                ESP_LOGI(TAG, "Received HeartsDistillingTempStabilized");
                auto event = reinterpret_cast<HeartsDistillingTempStabilizedEvent *>(event_p);
                Distiller::dispatch(*event);
                break;
            }
            case DistillerEventType::HeartsDistillingUnableToStabilizeTemp: {
                ESP_LOGI(TAG, "Received HeartsDistillingUnableToStabilizeTemp");
                auto event = reinterpret_cast<HeartsDistillingUnableToStabilizeTempEvent *>(event_p);
                Distiller::dispatch(*event);
                break;
            }
//            case DistillerEventType::TurnOff: {
//                auto event = reinterpret_cast<TurnOff *>(event_p);
//                Distiller::dispatch(*event);
//            }
//                break;

            default:
                break;
        }
        delete (event_p);
    }
}

[[noreturn]] void sensor_column_task(void *) {
    ESP_LOGI(TAG, "starting column sensor update task\n");
    for (;;) {
        Distiller::columnSensor.update();
    }
    vTaskDelete(NULL);
}

[[noreturn]] void sensor_def_task(void *) {
    ESP_LOGI(TAG, "starting def sensor update task\n");
    for (;;) {
        Distiller::defSensor.update();
    }
    vTaskDelete(NULL);
}

[[noreturn]] void sensor_tank_task(void *) {
    ESP_LOGI(TAG, "starting tank sensor update task\n");
    for (;;) {
        Distiller::tankSensor.update();
    }
    vTaskDelete(NULL);
}

[[noreturn]] void dispatch_task(void *) {
    ESP_LOGI(TAG, "starting dispatch_task\n");
    for (;;) {
        dispatch_queue_event();
    }
    vTaskDelete(NULL);
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "initializing hardware");
    sh1106_128x64_i2c_init();
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen();

    init_buttons();
    Distiller::buzzer.init(BUZZER_GPIO);

    ESP_LOGI(TAG, "initializing distiller state machine");

    Distiller::start();

    ESP_LOGI(TAG, "creating tasks");

    xTaskCreate(dispatch_task, "dispatch_task", configMINIMAL_STACK_SIZE * 20, nullptr, 5, nullptr);
    xTaskCreate(sensor_def_task, "sensor_def_task", configMINIMAL_STACK_SIZE * 5, nullptr, 5, nullptr);
    xTaskCreate(sensor_column_task, "sensor_column_task", configMINIMAL_STACK_SIZE * 5, nullptr, 5, nullptr);
    xTaskCreate(sensor_tank_task, "sensor_tank_task", configMINIMAL_STACK_SIZE * 5, nullptr, 5, nullptr);
}
