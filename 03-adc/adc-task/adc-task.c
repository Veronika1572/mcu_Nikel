#include "adc-task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Константы для работы с АЦП (внешний пин)
const uint ADC_GPIO_PIN = 26;      // GPIO 26 (соответствует ADC0)
const uint ADC_CHANNEL = 0;         // Канал АЦП 0 (соответствует GPIO 26)
const float ADC_REFERENCE_VOLTAGE = 3.3f;  // Опорное напряжение 3.3 В
const uint ADC_RESOLUTION = 4096;   // 12-битный АЦП (2^12 = 4096)

// Константы для датчика температуры
const uint TEMP_SENSOR_CHANNEL = 4;  // Канал АЦП для встроенного датчика температуры
const float TEMP_OFFSET_VOLTAGE = 0.706f;  // Напряжение при 27°C (из datasheet)
const float TEMP_SLOPE = 0.001721f;        // Коэффициент преобразования В/°C (из datasheet)

// НОВЫЕ ПЕРЕМЕННЫЕ ДЛЯ ТЕЛЕМЕТРИИ
static adc_task_state_t adc_state = ADC_TASK_STATE_IDLE;  // Текущее состояние
static uint64_t adc_ts = 0;                                // Метка времени для периодических измерений
const uint64_t ADC_TASK_MEAS_PERIOD_US = 100000;           // Период измерений 100 мс (100 000 мкс)

// Функция инициализации АЦП
void adc_task_init(void)
{
    // Инициализация периферийного модуля АЦП
    adc_init();
    
    // Инициализация GPIO для работы с АЦП (внешний пин)
    adc_gpio_init(ADC_GPIO_PIN);
    
    // Включение встроенного датчика температуры
    adc_set_temp_sensor_enabled(true);
    
    // Инициализация переменных телеметрии
    adc_state = ADC_TASK_STATE_IDLE;
    adc_ts = 0;
    
    printf("ADC task initialized:\n");
    printf("  External ADC: GPIO %d, Channel %d\n", ADC_GPIO_PIN, ADC_CHANNEL);
    printf("  Temperature sensor: Channel %d (internal)\n", TEMP_SENSOR_CHANNEL);
    printf("  Reference voltage: %.1f V, Resolution: %d bits\n", 
           ADC_REFERENCE_VOLTAGE, 12);
    printf("  Telemetry period: %llu ms (%llu us)\n", 
           ADC_TASK_MEAS_PERIOD_US / 1000, ADC_TASK_MEAS_PERIOD_US);
    printf("  Current state: %s\n", adc_state == ADC_TASK_STATE_IDLE ? "IDLE" : "RUN");
}

// НОВАЯ ФУНКЦИЯ: установка состояния
void adc_task_set_state(adc_task_state_t state)
{
    adc_state = state;
    
    // Сбрасываем метку времени при смене состояния
    adc_ts = time_us_64();
    
    printf("ADC task state changed to: %s\n", 
           adc_state == ADC_TASK_STATE_IDLE ? "IDLE" : "RUN");
}

// НОВАЯ ФУНКЦИЯ: обработчик телеметрии
void adc_task_handle(void)
{
    // Проверяем состояние модуля
    if (adc_state == ADC_TASK_STATE_IDLE)
    {
        // В состоянии IDLE ничего не делаем
        return;
    }
    
    // В состоянии RUN проверяем, не пора ли сделать измерение
    uint64_t now = time_us_64();
    if (now > adc_ts)
    {
        // Пора делать измерение
        
        // Получаем uptime в миллисекундах (время с запуска МК)
        uint32_t uptime_ms = to_ms_since_boot(get_absolute_time());
        
        // Измеряем напряжение
        float voltage_V = adc_task_read_voltage();
        
        // Измеряем температуру
        float temp_C = adc_task_read_temperature();
        
        // Выводим в формате: uptime(мс) напряжение(В) температура(°C)
        // Формат должен быть удобен для парсинга Python-скриптами
        printf("%u %.3f %.2f\n", uptime_ms, voltage_V, temp_C);
        
        // Устанавливаем следующую метку времени
        adc_ts = now + ADC_TASK_MEAS_PERIOD_US;
    }
}

// Функция измерения напряжения на внешнем пине
float adc_task_read_voltage(void)
{
    // Выбираем канал АЦП для измерения (внешний пин)
    adc_select_input(ADC_CHANNEL);
    
    // Запускаем измерение и читаем значение
    uint16_t voltage_counts = adc_read();
    
    // Преобразуем отсчеты в напряжение
    // Формула: V = (counts / 4095) * 3.3
    float voltage_V = (voltage_counts * ADC_REFERENCE_VOLTAGE) / (ADC_RESOLUTION - 1);
    
    return voltage_V;
}

// Функция измерения температуры чипа
float adc_task_read_temperature(void)
{
    // Выбираем канал АЦП для измерения (встроенный датчик температуры)
    adc_select_input(TEMP_SENSOR_CHANNEL);
    
    // Запускаем измерение и читаем значение
    uint16_t temp_counts = adc_read();
    
    // Преобразуем отсчеты в напряжение
    float temp_V = (temp_counts * ADC_REFERENCE_VOLTAGE) / (ADC_RESOLUTION - 1);
    
    // Преобразуем напряжение в температуру по формуле из datasheet RP2040
    // Формула: T = 27 - (V - 0.706) / 0.001721
    float temp_C = 27.0f - (temp_V - TEMP_OFFSET_VOLTAGE) / TEMP_SLOPE;
    
    return temp_C;
}
