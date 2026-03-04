#include "led-task.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdint.h>  // Для uint32_t

// Пины и периоды
const uint LED_PIN = 25;              // Пин встроенного светодиода на Pico
// uint LED_BLINK_PERIOD_US = 500000;  // Было: константа

// ТЕПЕРЬ ЭТО НЕ КОНСТАНТА, А ПЕРЕМЕННАЯ!
uint LED_BLINK_PERIOD_US = 500000;     // Период мигания 500 мс (полпериода = 250 мс)

// Переменные состояния
static uint64_t led_ts;                // Метка времени для мигания
static led_state_t led_state;           // Текущее состояние светодиода
static bool led_physical_state;         // Физическое состояние светодиода (для мигания)

// Функция инициализации
void led_task_init(void)
{
    // Устанавливаем начальное состояние
    led_state = LED_STATE_OFF;
    led_physical_state = false;
    
    // Обнуляем метку времени
    led_ts = 0;
    
    // Инициализируем пин светодиода
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, false);  // Начинаем с выключенным светодиодом
    
    printf("LED task initialized with period %d us\n", LED_BLINK_PERIOD_US);
}

// Функция установки состояния
void led_task_state_set(led_state_t state)
{
    led_state = state;
    
    // Сбрасываем метку времени при смене состояния
    led_ts = time_us_64();
    
    // При переходе в OFF или ON сразу устанавливаем физическое состояние
    if (state == LED_STATE_OFF)
    {
        gpio_put(LED_PIN, false);
        led_physical_state = false;
    }
    else if (state == LED_STATE_ON)
    {
        gpio_put(LED_PIN, true);
        led_physical_state = true;
    }
    
    printf("LED state changed to: %d\n", state);
}

// НОВАЯ ФУНКЦИЯ: установка периода мигания в миллисекундах
void led_task_set_blink_period_ms(uint32_t period_ms)
{
    // Проверка на допустимые значения
    if (period_ms < 10)
    {
        printf("Warning: period %d ms is too small, setting to 10 ms\n", period_ms);
        period_ms = 10;
    }
    
    // Переводим миллисекунды в микросекунды и сохраняем
    LED_BLINK_PERIOD_US = period_ms * 1000;
    
    // Сбрасываем метку времени для плавного перехода
    led_ts = time_us_64();
    
    printf("LED blink period set to %d ms (%d us)\n", period_ms, LED_BLINK_PERIOD_US);
}

// Функция обработчика (неблокирующая машина состояний)
void led_task_handle(void)
{
    switch (led_state)
    {
        case LED_STATE_OFF:
            // В состоянии OFF светодиод всегда выключен
            // Ничего не делаем, состояние уже установлено в led_task_state_set
            break;
            
        case LED_STATE_ON:
            // В состоянии ON светодиод всегда включен
            // Ничего не делаем, состояние уже установлено в led_task_state_set
            break;
            
        case LED_STATE_BLINK:
            // Неблокирующее мигание с использованием текущего периода
            if (time_us_64() > led_ts)
            {
                // Переключаем состояние светодиода
                led_physical_state = !led_physical_state;
                gpio_put(LED_PIN, led_physical_state);
                
                // Устанавливаем следующую метку времени (половина периода)
                led_ts = time_us_64() + (LED_BLINK_PERIOD_US / 2);
            }
            break;
            
        default:
            break;
    }
}
