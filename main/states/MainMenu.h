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

#ifndef MAGIC_DISTILLER_MAINMENU_H
#define MAGIC_DISTILLER_MAINMENU_H

#include "Distiller.h"
#include "../hw/DisplayHelper.h"
#include "Buzzer.h"
#include "Rectification/InitialStabilization.h"
#include "ssd1306.h"
#include "SystemInfoScreen.h"

class SystemInfoScreen;

class MainMenu : public Distiller {
public:
    /* Define menu items of the menu box */
    static const char *menuItems[6];

    /* This variable will hold menu state, processed by SSD1306 API functions */
    SAppMenu menu;

    MainMenu() {
        ESP_LOGI(TAG,"Calling MainMenu constructor");
        ssd1306_createMenu(&menu, MainMenu::menuItems, sizeof(MainMenu::menuItems) / sizeof(char *));
    }

    void entry() override {
        DisplayHelper::Clear();

        ssd1306_showMenu(&menu);
    }

    void react(ButtonClickDistillerEvent const &event) override {
        switch (event.button()) {
            case DOWN:
                ssd1306_menuDown(&menu);
                ssd1306_updateMenu(&menu);
                buzzer.beep_btn();
                break;
            case UP:
                ssd1306_menuUp(&menu);
                ssd1306_updateMenu(&menu);
                buzzer.beep_btn();
                break;
            case ENTER:
                switch (menu.selection) {
                    case 2:
                        buzzer.beep_btn();
                        transit<InitialStabilization>();
                        break;
                    case 5:
                        buzzer.beep_btn();
                        transit<SystemInfoScreen>();
                        break;
                    default:
                        buzzer.beep_bad();
                        break;
                }
                break;
            default:
                buzzer.beep_bad();
                break;
        }
    }
};

#endif //MAGIC_DISTILLER_MAINMENU_H
