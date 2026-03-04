#include <stdio.h>
#include <stdint.h>  // Добавляем для uint32_t
#include "pico/stdlib.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"

// Определяем имя устройства и версию
#define DEVICE_NAME "my-pico-device"
#define DEVICE_VERSION "v0.5.0"

// Колбэк для команды version
void version_callback(const char* args)
{
    printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VERSION);
    
    // Если есть аргументы, можно их обработать
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
    
    // Можно добавить обработку аргументов для изменения периода
    if (args && args[0] != '\0')
    {
        printf("Arguments ignored: '%s' (use 'set_period' command to change period)\n", args);
    }
}

// НОВЫЙ КОЛБЭК: установка периода мигания
void led_blink_set_period_ms_callback(const char* args)
{
    // Проверка на наличие аргументов
    if (!args || args[0] == '\0')
    {
        printf("Error: period not specified. Usage: set_period <milliseconds>\n");
        return;
    }
    
    // Парсим аргумент как целое число
    uint period_ms = 0;
    int parsed = sscanf(args, "%u", &period_ms);
    
    // Проверка успешности парсинга
    if (parsed != 1 || period_ms == 0)
    {
        printf("Error: invalid period '%s'. Please specify positive integer.\n", args);
        return;
    }
    
    // Устанавливаем новый период
    led_task_set_blink_period_ms(period_ms);
    
    // Если светодиод не в режиме мигания, предлагаем его включить
    printf("Period set to %d ms. Use 'blink' command to start blinking.\n", period_ms);
}

// Массив команд - последний элемент должен быть нулевым
api_t device_api[] =
{
    {"version", version_callback, "get device name and firmware version"},
    {"on", led_on_callback, "turn LED on"},
    {"off", led_off_callback, "turn LED off"},
    {"blink", led_blink_callback, "make LED blink"},
    {"set_period", led_blink_set_period_ms_callback, "set blink period in milliseconds"},  // НОВАЯ КОМАНДА
    {NULL, NULL, NULL},  // Маркер конца массива
};

int main() {
    // Инициализация stdio (для USB)
    stdio_init_all();
    
    // Небольшая задержка для стабилизации USB-соединения
    sleep_ms(2000);
    
    // Инициализация модулей
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    
    // Вывод приветственного сообщения
    printf("\n========================================\n");
    printf("02-control program started\n");
    printf("Device: %s, Version: %s\n", DEVICE_NAME, DEVICE_VERSION);
    printf("Type a command and press Enter:\n");
    printf("Available commands:\n");
    
    // Выводим список доступных команд
    for (int i = 0; device_api[i].command_name != NULL; i++)
    {
        printf("  %s - %s\n", device_api[i].command_name, device_api[i].command_help);
    }
    
    printf("========================================\n\n");
    printf("> ");
    fflush(stdout);
    
    // Бесконечный цикл - БЕЗ ЗАДЕРЖЕК!
    while (true) {
        // Получаем команду от модуля ввода
        char* command_string = stdio_task_handle();
        
        // Если команда получена, передаем ее в модуль протокола
        if (command_string != NULL)
        {
            protocol_task_handle(command_string);
            printf("\n> ");  // Приглашение для следующей команды
            fflush(stdout);
        }
        
        // Регулярно вызываем обработчик светодиода
        led_task_handle();
        
        // Никаких задержек - процессор постоянно проверяет ввод и состояние LED
    }
    
    return 0;
}
