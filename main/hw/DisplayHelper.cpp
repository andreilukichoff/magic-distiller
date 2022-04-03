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
#include "DisplayHelper.h"
#include "lcd/oled_sh1106.h"
#include "ssd1306.h"
#include "nano_gfx.h"
#include "../common.h"
#include "Buzzer.h"
#include "../states/Distiller.h"

void DisplayHelper::Animate(uint8_t x, uint8_t y, const char *text, const uint8_t time_ms) {
    for (uint8_t i = 0; i < strlen(text); i++) {
        ssd1306_setCursor(x + i * 6, y);
        char ch[2] = {text[i], 0x00};
        ssd1306_print(ch);
        Distiller::buzzer.beep(Buzzer::NOTE_B3, time_ms / 4);
        DELAY((time_ms / 4) * 3);
    }
}

void DisplayHelper::Print(uint8_t x, uint8_t y, const char *text) {
    char formatted[22];
    strcpy(formatted, text);
    for (int i = strlen(text); i < 21; i++)
        formatted[i] = 0x20;

    formatted[21] = 0;

    ssd1306_printFixed(x, y, text, STYLE_NORMAL);
}

void DisplayHelper::PrintFmt(uint8_t x, uint8_t y, const char *text, ...) {
    va_list args;
    va_start(args, text);

    char formatted[22];// = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    //   0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0};
    int written = vsnprintf(formatted, 21, text, args);
    for (int i = written; i < 21; i++)
        formatted[i] = 0x20;

    formatted[21] = 0;

    ssd1306_printFixed(x, y, formatted, STYLE_NORMAL);

    va_end(args);
}

void DisplayHelper::PrintBold(uint8_t x, uint8_t y, const char *text) {
    ssd1306_printFixed(x, y, text, STYLE_BOLD);
}

void DisplayHelper::Clear() {
    ssd1306_clearScreen();
}

void DisplayHelper::ClearRow(uint8_t row) {
    Print(0, row, "                    ");
}
