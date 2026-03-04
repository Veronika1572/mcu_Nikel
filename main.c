#include "pico/stdlib.h"
#include "hardware/gpio.h"

const unsigned int LED_PIN = 25;

int main()
{
    // Инициализация GPIO
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Бесконечный цикл
    while (1)
    {
        gpio_put(LED_PIN, 1); // Включить LED
        sleep_ms(250);        // Подождать 250 мс
        gpio_put(LED_PIN, 0); // Выключить LED
        sleep_ms(1000);       // Подождать 1 секунду
    }

    return 0; // Никогда не выполнится
}