#include "protocol-task.h"
#include <stdio.h>
#include <string.h>

// Статические переменные для хранения указателя на массив команд и его размера
static api_t* api = {0};
static int commands_count = 0;

// Функция инициализации модуля
void protocol_task_init(api_t* device_api)
{
    // Сохраняем указатель на массив команд
    api = device_api;
    
    // Подсчитываем количество команд
    commands_count = 0;
    
    // Проходим по массиву, пока не встретим NULL в имени команды
    while (api[commands_count].command_name != NULL)
    {
        commands_count++;
    }
    
    printf("Protocol task initialized with %d commands\n", commands_count);
}

// Функция обработки команды
void protocol_task_handle(char* command_string)
{
    // Проверка на NULL - строка команды еще не получена
    if (!command_string)
    {
        return;
    }
    
    // Логика обработки полученной строки
    // Делим строку на команду и аргументы
    const char* command_name = command_string;
    const char* command_args = NULL;
    
    // Ищем пробел в строке
    char* space_symbol = strchr(command_string, ' ');
    
    if (space_symbol)
    {
        // Разделяем строку: заменяем пробел на нулевой символ
        *space_symbol = '\0';
        command_args = space_symbol + 1;
    }
    else
    {
        // Если пробела нет, аргументы - пустая строка
        command_args = "";
    }
    
    // Выводим найденные имя команды и аргументы
    printf("Parsed command: '%s', args: '%s'\n", command_name, command_args);
    
    // В цикле проходим по массиву команд и ищем совпадение
    for (int i = 0; i < commands_count; i++)
    {
        // Сравниваем имя команды с именем в массиве
        if (strcmp(command_name, api[i].command_name) == 0)
        {
            // Нашли команду - вызываем callback
            api[i].command_callback(command_args);
            return;
        }
    }
    
    // Если команда не найдена, выводим ошибку
    printf("Error: unknown command '%s'\n", command_name);
    printf("Available commands:\n");
    for (int i = 0; i < commands_count; i++)
    {
        printf("  %s - %s\n", api[i].command_name, api[i].command_help);
    }
    
    return;
}
