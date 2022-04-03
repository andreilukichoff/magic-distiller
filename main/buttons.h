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

#ifndef MAGIC_DISTILLER_BUTTONS_H
#define MAGIC_DISTILLER_BUTTONS_H

#include "common.h"
#include "states/Distiller.h"

static button_t btn_left, btn_right, btn_up, btn_down, btn_enter, btn_exit;

static void on_button(button_t *btn, button_state_t state) {
    if (state == BUTTON_PRESSED) {
        ESP_LOGI("on_button", "button click, GPIO: %d", btn->gpio);
        Button t;

        switch(btn->gpio) {
            case BUTTON_ENTER_GPIO:
                t = ENTER;
                break;
            case BUTTON_EXIT_GPIO:
                t = EXIT;
                break;
            case BUTTON_LEFT_GPIO:
                t = LEFT;
                break;
            case BUTTON_RIGHT_GPIO:
                t = RIGHT;
                break;
            case BUTTON_UP_GPIO:
                t = UP;
                break;
            case BUTTON_DOWN_GPIO:
                t = DOWN;
                break;
            default:
                t = UNKNOWN;
                break;
        }
        auto *event = new ButtonClickDistillerEvent(t);

        ESP_LOGI("on_button", "sending ButtonClickDistillerEvent with button: %d", t);
        xQueueSend(Distiller::eventQueue, &event, 5 / portTICK_PERIOD_MS);
    }
}

static void init_buttons() {
    btn_left.gpio = BUTTON_LEFT_GPIO;
    btn_left.pressed_level = 0;
    btn_left.internal_pull = true;
    btn_left.autorepeat = false;
    btn_left.callback = on_button;

    ESP_ERROR_CHECK(button_init(&btn_left));

    btn_right.gpio = BUTTON_RIGHT_GPIO;
    btn_right.pressed_level = 0;

    btn_right.internal_pull = true;
    btn_right.autorepeat = false;
    btn_right.callback = on_button;

    ESP_ERROR_CHECK(button_init(&btn_right));

    btn_exit.gpio = BUTTON_EXIT_GPIO;
    btn_exit.pressed_level = 0;

    btn_exit.internal_pull = true;
    btn_exit.autorepeat = false;
    btn_exit.callback = on_button;

    ESP_ERROR_CHECK(button_init(&btn_exit));

    btn_enter.gpio = BUTTON_ENTER_GPIO;
    btn_enter.pressed_level = 0;

    btn_enter.internal_pull = true;
    btn_enter.autorepeat = false;
    btn_enter.callback = on_button;

    ESP_ERROR_CHECK(button_init(&btn_enter));

    btn_up.gpio = BUTTON_UP_GPIO;
    btn_up.pressed_level = 0;

    btn_up.internal_pull = true;
    btn_up.autorepeat = false;
    btn_up.callback = on_button;

    ESP_ERROR_CHECK(button_init(&btn_up));

    btn_down.gpio = BUTTON_DOWN_GPIO;
    btn_down.pressed_level = 0;

    btn_down.internal_pull = true;
    btn_down.autorepeat = false;
    btn_down.callback = on_button;

    ESP_ERROR_CHECK(button_init(&btn_down));
}

#endif //MAGIC_DISTILLER_BUTTONS_H
