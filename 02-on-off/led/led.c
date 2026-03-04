#include "led.h"
#include "hardware/gpio.h"

// Константа - номер пина, к которому подключен светодиод
// На Raspberry Pi Pico встроенный светодиод обычно на GPIO 25
#define LED_PIN 25

void led_init(void) {
    // Инициализация GPIO пина
    gpio_init(LED_PIN);
    // Настройка пина как выход
    gpio_set_dir(LED_PIN, GPIO_OUT);
    // Начальное состояние - выключен
    gpio_put(LED_PIN, 0);
}

void led_enable(void) {
    // Включение светодиода (установка высокого уровня на пине)
    gpio_put(LED_PIN, 1);
}

void led_disable(void) {
    // Выключение светодиода (установка низкого уровня на пине)
    gpio_put(LED_PIN, 0);
}
