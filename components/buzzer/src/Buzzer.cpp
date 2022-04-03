#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Buzzer.h"

void Buzzer::init(gpio_num_t gpio) {
    gpio_ = gpio;

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << gpio_);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(gpio_, 0);
}

void Buzzer::beep(piano_note_t key_num, uint16_t dur_hms) {
    periph_module_enable(PERIPH_LEDC_MODULE);
    ledc_timer_config_t ledc_timer;
    ledc_channel_config_t ledc_channel;

    ledc_timer.freq_hz = notes[key_num];                        // frequency of PWM signal
    ledc_timer.duty_resolution = LEDC_TIMER_13_BIT;    // resolution of PWM duty
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;            // timer mode
    ledc_timer.timer_num = LEDC_TIMER_0;            // timer index
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;

    // Set configuration of timer0 for high speed channels

    ledc_channel.channel = LEDC_CHANNEL_0;
    ledc_channel.duty = 4095;
    ledc_channel.gpio_num = gpio_;
    ledc_channel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_channel.hpoint = 0;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;

    ledc_timer_config(&ledc_timer);
    ledc_channel_config(&ledc_channel);

    vTaskDelay(pdMS_TO_TICKS(dur_hms));
    ledc_stop(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
}

void Buzzer::beep_btn() {
    beep(NOTE_B5, 25);
}

void Buzzer::beep_bad() {
    beep(NOTE_B3, 25);
    vTaskDelay(pdMS_TO_TICKS(10));
    beep(NOTE_B3, 25);
    vTaskDelay(pdMS_TO_TICKS(10));
    beep(NOTE_B3, 25);
}

void Buzzer::beep_toobad() {
    for (int i = 0; i < 3; ++i) {
        beep(NOTE_B6, 1000);
        beep(NOTE_B4, 1000);
    }
}

void Buzzer::beep_attention() {
    for (int n = 0; n < 3; ++n) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                beep(NOTE_B5, 50);
                vTaskDelay(pdMS_TO_TICKS(10));
                beep(NOTE_B6, 50);
                vTaskDelay(pdMS_TO_TICKS(20));
            }
            vTaskDelay(pdMS_TO_TICKS(250));
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

void Buzzer::beep_good() {
    for (int i = 0; i < 1; ++i) {
        beep(NOTE_B4, 125);
        vTaskDelay(pdMS_TO_TICKS(50));
        beep(NOTE_C5, 125);
        vTaskDelay(pdMS_TO_TICKS(50));
        beep(NOTE_D5, 125);
        vTaskDelay(pdMS_TO_TICKS(50));
        beep(NOTE_E5, 125);
        vTaskDelay(pdMS_TO_TICKS(50));
        beep(NOTE_F5, 125);
        vTaskDelay(pdMS_TO_TICKS(50));
        beep(NOTE_G5, 125);
        vTaskDelay(pdMS_TO_TICKS(50));
        beep(NOTE_A5, 125);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}