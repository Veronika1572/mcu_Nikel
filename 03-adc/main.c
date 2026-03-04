#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "adc-task/adc-task.h"

// Определяем имя устройства и версию
#define DEVICE_NAME "my-pico-device"
#define DEVICE_VERSION "v0.10.0"

// ПРОТОТИПЫ ФУНКЦИЙ
void version_callback(const char* args);
void led_on_callback(const char* args);
void led_off_callback(const char* args);
void led_blink_callback(const char* args);
void led_blink_set_period_ms_callback(const char* args);
void help_callback(const char* args);
void mem_read_callback(const char* args);
void mem_write_callback(const char* args);
void adc_read_callback(const char* args);
void temp_read_callback(const char* args);
void tm_start_callback(const char* args);   // НОВЫЙ ПРОТОТИП
void tm_stop_callback(const char* args);    // НОВЫЙ ПРОТОТИП

// Массив команд
api_t device_api[] =
{
    {"help", help_callback, "show this help message"},
    {"version", version_callback, "get device name and firmware version"},
    {"on", led_on_callback, "turn LED on"},
    {"off", led_off_callback, "turn LED off"},
    {"blink", led_blink_callback, "make LED blink"},
    {"set_period", led_blink_set_period_ms_callback, "set blink period in milliseconds"},
    {"mem", mem_read_callback, "read memory at address (hex): mem 0x20000000"},
    {"wmem", mem_write_callback, "write memory at address (hex): wmem 0x20000000 0x1234"},
    {"get_adc", adc_read_callback, "read ADC voltage from GPIO 26"},
    {"get_temp", temp_read_callback, "read RP2040 internal temperature"},
    {"tm_start", tm_start_callback, "start telemetry (continuous measurements)"},  // НОВАЯ КОМАНДА
    {"tm_stop", tm_stop_callback, "stop telemetry"},  // НОВАЯ КОМАНДА
    {NULL, NULL, NULL},
};

// ... (все существующие callback'и остаются без изменений)

// НОВЫЙ КОЛБЭК: запуск телеметрии
void tm_start_callback(const char* args)
{
    adc_task_set_state(ADC_TASK_STATE_RUN);
    printf("Telemetry started (period = 100 ms)\n");
    printf("Format: uptime(ms) voltage(V) temperature(C)\n");
    
    if (args && args[0] != '\0')
    {
        printf("Arguments ignored: '%s'\n", args);
    }
}

// НОВЫЙ КОЛБЭК: остановка телеметрии
void tm_stop_callback(const char* args)
{
    adc_task_set_state(ADC_TASK_STATE_IDLE);
    printf("Telemetry stopped\n");
    
    if (args && args[0] != '\0')
    {
        printf("Arguments ignored: '%s'\n", args);
    }
}

int main() {
    // Инициализация stdio (для USB)
    stdio_init_all();
    
    // Небольшая задержка для стабилизации USB-соединения
    sleep_ms(2000);
    
    // Инициализация модулей
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    adc_task_init();
    
    // Вывод приветственного сообщения
    printf("\n========================================\n");
    printf("03-adc program started\n");
    printf("Device: %s, Version: %s\n", DEVICE_NAME, DEVICE_VERSION);
    printf("ADC module: GPIO 26, Channel 0, 12-bit, 3.3V reference\n");
    printf("Temperature sensor: internal, Channel 4\n");
    printf("Telemetry: tm_start / tm_stop commands\n");
    printf("Type 'help' for list of commands\n");
    printf("========================================\n\n");
    printf("> ");
    fflush(stdout);
    
    // Бесконечный цикл
    while (true) {
        // Получаем команду от модуля ввода
        char* command_string = stdio_task_handle();
        
        // Если команда получена, передаем ее в модуль протокола
        if (command_string != NULL)
        {
            protocol_task_handle(command_string);
            printf("\n> ");
            fflush(stdout);
        }
        
        // Регулярно вызываем обработчики
        led_task_handle();
        adc_task_handle();  // ВАЖНО: вызываем обработчик телеметрии
    }
    
    return 0;
}
