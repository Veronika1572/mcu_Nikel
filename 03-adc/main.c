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
void help_callback(const char* args);
void version_callback(const char* args);
void led_on_callback(const char* args);
void led_off_callback(const char* args);
void led_blink_callback(const char* args);
void led_blink_set_period_ms_callback(const char* args);
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

// Колбэк для команды help
void help_callback(const char* args)
{
    printf("\n========================================\n");
    printf("Available commands:\n");
    printf("========================================\n");
    
    for (int i = 0; device_api[i].command_name != NULL; i++)
    {
        printf("  %-12s - %s\n", device_api[i].command_name, device_api[i].command_help);
    }
    
    printf("========================================\n");
    
    if (args && args[0] != '\0')
    {
        printf("Note: 'help' command ignores arguments: '%s'\n", args);
    }
}

// Колбэк для команды version
void version_callback(const char* args)
{
    printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VERSION);
    
    if (args && args[0] != '\0')
    {
        printf("Command arguments: '%s'\n", args);
    }
}

// Колбэк для команды on (включить светодиод)
void led_on_callback(const char* args)
{
    led_task_state_set(LED_STATE_ON);
    printf("LED turned ON\n");
    
    if (args && args[0] != '\0')
    {
        printf("Arguments ignored: '%s'\n", args);
    }
}

// Колбэк для команды off (выключить светодиод)
void led_off_callback(const char* args)
{
    led_task_state_set(LED_STATE_OFF);
    printf("LED turned OFF\n");
    
    if (args && args[0] != '\0')
    {
        printf("Arguments ignored: '%s'\n", args);
    }
}

// Колбэк для команды blink (мигать светодиодом)
void led_blink_callback(const char* args)
{
    led_task_state_set(LED_STATE_BLINK);
    printf("LED blinking started\n");
    
    if (args && args[0] != '\0')
    {
        printf("Arguments ignored: '%s' (use 'set_period' command to change period)\n", args);
    }
}

// Колбэк для установки периода мигания
void led_blink_set_period_ms_callback(const char* args)
{
    if (!args || args[0] == '\0')
    {
        printf("Error: period not specified. Usage: set_period <milliseconds>\n");
        return;
    }
    
    uint period_ms = 0;
    int parsed = sscanf(args, "%u", &period_ms);
    
    if (parsed != 1 || period_ms == 0)
    {
        printf("Error: invalid period '%s'. Please specify positive integer.\n", args);
        return;
    }
    
    led_task_set_blink_period_ms(period_ms);
    printf("Period set to %d ms. Use 'blink' command to start blinking.\n", period_ms);
}

// Колбэк для чтения памяти
void mem_read_callback(const char* args)
{
    if (!args || args[0] == '\0')
    {
        printf("Error: address not specified. Usage: mem <hex_address>\n");
        printf("Example: mem 0x20000000\n");
        return;
    }
    
    uint32_t addr = 0;
    int parsed = sscanf(args, "%x", &addr);
    
    if (parsed != 1)
    {
        printf("Error: invalid address '%s'. Please specify hex value.\n", args);
        return;
    }
    
    volatile uint32_t* ptr = (volatile uint32_t*)addr;
    uint32_t value = *ptr;
    
    printf("Memory read:\n");
    printf("  Address: 0x%08X\n", addr);
    printf("  Value:   0x%08X (decimal: %u)\n", value, value);
}

// Колбэк для записи в память
void mem_write_callback(const char* args)
{
    if (!args || args[0] == '\0')
    {
        printf("Error: arguments not specified. Usage: wmem <hex_address> <hex_value>\n");
        printf("Example: wmem 0xd0000014 0x02000000\n");
        return;
    }
    
    uint32_t addr = 0;
    uint32_t value = 0;
    int parsed = sscanf(args, "%x %x", &addr, &value);
    
    if (parsed != 2)
    {
        printf("Error: invalid arguments '%s'. Need address and value in hex.\n", args);
        return;
    }
    
    volatile uint32_t* ptr = (volatile uint32_t*)addr;
    *ptr = value;
    
    uint32_t read_back = *ptr;
    
    printf("Memory write:\n");
    printf("  Address: 0x%08X\n", addr);
    printf("  Written: 0x%08X\n", value);
    printf("  Read back: 0x%08X\n", read_back);
}

// Колбэк для чтения АЦП
void adc_read_callback(const char* args)
{
    float voltage = adc_task_read_voltage();
    printf("%f\n", voltage);
    
    if (args && args[0] != '\0')
    {
        // Игнорируем аргументы
    }
}

// Колбэк для чтения температуры
void temp_read_callback(const char* args)
{
    float temperature = adc_task_read_temperature();
    printf("%f\n", temperature);
    
    if (args && args[0] != '\0')
    {
        // Игнорируем аргументы
    }
}

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
