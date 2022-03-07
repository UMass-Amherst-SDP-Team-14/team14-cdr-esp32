#include <driver/gpio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lora/lora.h"
#include "gps/gps.h"

// LORA Pins
#define LORA_SSPIN GPIO_NUM_10
#define LORA_RSTPIN GPIO_NUM_9
#define LORA_DIO0PIN GPIO_NUM_2

// General Pins
#define LED_PIN GPIO_NUM_5
#define BTN_PIN GPIO_NUM_6
#define GPS_CTRL_PIN GPIO_NUM_4

int initialize() {
    // configure output pins
    gpio_config_t io_conf;

    // output pins
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pin_bit_mask = (1ULL << LED_PIN || 1ULL << GPS_CTRL_PIN);
    gpio_config(&io_conf);

    // input pins
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pin_bit_mask = (1ULL << BTN_PIN);
    gpio_config(&io_conf);

    // LORA
    lora_init();
    lora_set_frequency(915e6);
    lora_enable_crc();

    return 0;
}