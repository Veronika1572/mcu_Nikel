#pragma once

#include <stdint.h>  // Добавляем для uint32_t

// Перечисление возможных состояний светодиода
typedef enum
{
    LED_STATE_OFF = 0,    // Светодиод выключен
    LED_STATE_ON = 1,     // Светодиод включен
    LED_STATE_BLINK = 2,  // Светодиод мигает
} led_state_t;

// Прототипы функций модуля led-task
void led_task_init(void);
void led_task_handle(void);
void led_task_state_set(led_state_t state);
void led_task_set_blink_period_ms(uint32_t period_ms);  // НОВАЯ ФУНКЦИЯ
