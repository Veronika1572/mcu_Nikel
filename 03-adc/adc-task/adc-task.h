#pragma once

#include <stdint.h>
#include <stdbool.h>

// Состояния модуля телеметрии
typedef enum
{
    ADC_TASK_STATE_IDLE = 0,  // Состояние ожидания (измерения не производятся)
    ADC_TASK_STATE_RUN = 1,    // Состояние измерений (телеметрия активна)
} adc_task_state_t;

// Прототипы функций модуля adc-task
void adc_task_init(void);
void adc_task_handle(void);                    // Функция обработчика (вызывается в цикле)
void adc_task_set_state(adc_task_state_t state); // Функция смены состояния
float adc_task_read_voltage(void);
float adc_task_read_temperature(void);
