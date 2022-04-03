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

#ifndef MAGIC_DISTILLER_GREETING_H
#define MAGIC_DISTILLER_GREETING_H

#include "Distiller.h"
#include "../hw/DisplayHelper.h"
#include "Buzzer.h"
#include "Rectification/InitialStabilization.h"
#include "SystemInfoScreen.h"
#include "MainMenu.h"

class Greeting : public Distiller {
    void entry() override {
        headsValve.Init();
        heartsValve.Init();
        DisplayHelper::Clear();

        DisplayHelper::Animate(0,8, "Magic distiller");
        DisplayHelper::Animate(0,16, "v0.3 @ 15.03.2022");
        DisplayHelper::Animate(0,32, "PRESS ANY BTN");
    }

    void react(ButtonClickDistillerEvent const &event) override {
        switch (event.button()) {
            default:
                buzzer.beep_btn();
                transit<MainMenu>();
                break;
        }
    }
};

#endif //MAGIC_DISTILLER_GREETING_H
