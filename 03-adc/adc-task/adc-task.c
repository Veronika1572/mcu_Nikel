#include "adc-task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Константы
const uint ADC_GPIO_PIN = 26;
const uint ADC_CHANNEL = 0;
const float ADC_REFERENCE_VOLTAGE = 3.3f;
const uint ADC_RESOLUTION = 4096;
const uint TEMP_SENSOR_CHANNEL = 4;
const float TEMP_OFFSET_VOLTAGE = 0.706f;
const float TEMP_SLOPE = 0.001721f;

// Переменные телеметрии
static adc_task_state_t adc_state = ADC_TASK_STATE_IDLE;
static uint64_t adc_ts = 0;
const uint64_t ADC_TASK_MEAS_PERIOD_US = 100000;

void adc_task_init(void)
{
    adc_init();
    adc_gpio_init(ADC_GPIO_PIN);
    adc_set_temp_sensor_enabled(true);
    
    adc_state = ADC_TASK_STATE_IDLE;
    adc_ts = 0;
    
    printf("ADC task initialized\n");
}

void adc_task_set_state(adc_task_state_t state)
{
    adc_state = state;
    adc_ts = time_us_64();
    printf("ADC state: %s\n", state == ADC_TASK_STATE_IDLE ? "IDLE" : "RUN");
}

void adc_task_handle(void)
{
    if (adc_state == ADC_TASK_STATE_IDLE)
    {
        return;
    }
    
    uint64_t now = time_us_64();
    if (now > adc_ts)
    {
        uint32_t uptime_ms = to_ms_since_boot(get_absolute_time());
        float voltage_V = adc_task_read_voltage();
        float temp_C = adc_task_read_temperature();
        
        printf("%u %.3f %.2f\n", uptime_ms, voltage_V, temp_C);
        
        adc_ts = now + ADC_TASK_MEAS_PERIOD_US;
    }
}

float adc_task_read_voltage(void)
{
    adc_select_input(ADC_CHANNEL);
    uint16_t voltage_counts = adc_read();
    return (voltage_counts * ADC_REFERENCE_VOLTAGE) / (ADC_RESOLUTION - 1);
}

float adc_task_read_temperature(void)
{
    adc_select_input(TEMP_SENSOR_CHANNEL);
    uint16_t temp_counts = adc_read();
    float temp_V = (temp_counts * ADC_REFERENCE_VOLTAGE) / (ADC_RESOLUTION - 1);
    return 27.0f - (temp_V - TEMP_OFFSET_VOLTAGE) / TEMP_SLOPE;
}
