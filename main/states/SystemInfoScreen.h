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

#ifndef MAGIC_DISTILLER_SYSTEMINFOSCREEN_H
#define MAGIC_DISTILLER_SYSTEMINFOSCREEN_H

//#include <esp_netif.h>
#include "Distiller.h"
#include "../hw/DisplayHelper.h"
#include "Buzzer.h"
#include "ssd1306.h"
#include "MainMenu.h"

class SystemInfoScreen : public Distiller {
    void entry() override {
        DisplayHelper::Clear();
        DisplayHelper::Print(0,8,"System info:");

//        tcpip_adapter_ip_info_t ipInfo;
//        tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);
//        DisplayHelper::PrintFmt(0,16,"IP: " IPSTR, IP2STR(&ipInfo.ip));

        DisplayHelper::PrintFmt(0,32,"any btn to return");
    }

    void react(ButtonClickDistillerEvent const &event) override {
        buzzer.beep_btn();
        switch (event.button()) {
            default:
                transit<MainMenu>();
                break;
        }
    }
};

#endif //MAGIC_DISTILLER_SYSTEMINFOSCREEN_H
