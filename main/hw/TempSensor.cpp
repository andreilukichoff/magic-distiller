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

#include <ds18x20.h>
#include <esp_log.h>
#include "TempSensor.h"
#include "freertos/FreeRTOS.h"

const char *TempSensor::TAG = "TempSensor";

TempSensor::TempSensor(gpio_num_t pin) {
    pin_ = pin;
    reset();
}

void TempSensor::update() {
    esp_err_t result;
    size_t found_sensors_count = 0;
    onewire_addr_t addrs[1]{0};
    bool success = false;

    result = ds18x20_scan_devices(pin_, addrs, 1, &found_sensors_count);
    if (result == ESP_OK) {
        if (found_sensors_count) {
            assert(found_sensors_count == 1);
            float current_temp;
            result = ds18x20_measure_and_read(pin_, addrs[0], &current_temp);
            if (result == ESP_OK) {
                temp(current_temp);
                success = true;
            } else {
                ESP_LOGE(TAG, "%s", esp_err_to_name(result));
                reset();
            }
        }
    } else {
        ESP_LOGE(TAG, "%s", esp_err_to_name(result));
        reset();
    }
    if (!success)
        vTaskDelay(pdMS_TO_TICKS(100));
}

void TempSensor::reset() {
    spinlock_t mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
    address_ = 0;
    temp_ = TEMP_UNDEFINED;
    bufferPos_ = 0;
    bufferFull = false;
    for (uint8_t i = 0; i < TEMP_BUFFER_SIZE; i++) {
        tempBuffer_[i] = TEMP_UNDEFINED;
    }
    portEXIT_CRITICAL(&mux);
}

void TempSensor::temp(const float current_temp) {
    tempBuffer_[bufferPos_] = current_temp;
    if (bufferPos_ == TEMP_BUFFER_SIZE - 1) {
        bufferPos_ = 0;
        bufferFull = true;
    } else
        bufferPos_++;

    if (bufferFull) {
        float sum = 0;
        for (__uint8_t i = 0; i < TEMP_BUFFER_SIZE; i++) {
            sum += tempBuffer_[i];
        }
        temp_ = sum / TEMP_BUFFER_SIZE;
    }
}