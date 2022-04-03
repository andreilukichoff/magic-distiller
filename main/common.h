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

#ifndef MAGIC_DISTILLER_COMMON_H
#define MAGIC_DISTILLER_COMMON_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <button.h>
#include "tinyfsm.hpp"

#define DELAY(ms) vTaskDelay(pdMS_TO_TICKS(ms))

#define BUZZER_GPIO GPIO_NUM_5

#define BUTTON_EXIT_GPIO   GPIO_NUM_19
#define BUTTON_LEFT_GPIO   GPIO_NUM_2
#define BUTTON_UP_GPIO   GPIO_NUM_4
#define BUTTON_DOWN_GPIO   GPIO_NUM_18
#define BUTTON_RIGHT_GPIO   GPIO_NUM_16
#define BUTTON_ENTER_GPIO   GPIO_NUM_17

#define VALVE_HEADS_GPIO GPIO_NUM_14
#define VALVE_HEARTS_GPIO GPIO_NUM_15

#define SENSOR_DEF_GPIO GPIO_NUM_25
#define SENSOR_COL_GPIO GPIO_NUM_33
#define SENSOR_TANK_GPIO GPIO_NUM_32

#define INITIAL_STABILIZE_DELTA 0.1f
#define INITIAL_STABILIZE_TIME MINUTES(10)
#define INITIAL_STABILIZE_TIMEOUT MINUTES(40)
#define HEARTS_STABILIZE_DELTA 0.1f
#define HEARTS_STABILIZE_TIMEOUT MINUTES(10)
#define HEARTS_STABILIZE_CONFIRM 8

#endif //MAGIC_DISTILLER_COMMON_H
