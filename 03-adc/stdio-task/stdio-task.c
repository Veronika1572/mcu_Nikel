#include "stdio-task.h"
#include <stdio.h>
#include "pico/stdlib.h"

// Размер буфера для команд
#define COMMAND_BUF_LEN 128

// Буфер для хранения введенной строки
char command[COMMAND_BUF_LEN] = {0};

// Текущий индекс в буфере
int command_buf_idx;

// Функция инициализации модуля
void stdio_task_init()
{
    command_buf_idx = 0;
}

// Функция обработки ввода с таймаутом
char* stdio_task_handle()
{
    // Пытаемся прочитать символ с таймаутом 0 (неблокирующий режим)
    int symbol = getchar_timeout_us(0);
    
    // Если символа нет, возвращаем NULL
    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }
    
    // Эхо-вывод символа обратно в терминал
    putchar(symbol);
    fflush(stdout);  // Принудительный вывод
    
    // Обработка символов перевода строки (Enter)
    if (symbol == '\r' || symbol == '\n')
    {
        // Завершаем строку нулевым символом
        command[command_buf_idx] = '\0';
        command_buf_idx = 0;
        
        // Выводим полученную строку (для отладки)
        // printf("received string: '%s'\n", command);
        
        // Возвращаем указатель на команду
        return command;
    }
    
    // Обработка backspace (удаление символа)
    if (symbol == '\b' || symbol == 127)
    {
        if (command_buf_idx > 0)
        {
            command_buf_idx--;
            // Для красивого удаления в терминале
            printf(" \b");
            fflush(stdout);
        }
        return NULL;
    }
    
    // Проверка переполнения буфера
    if (command_buf_idx >= COMMAND_BUF_LEN - 1)
    {
        // Сбрасываем буфер при переполнении
        command_buf_idx = 0;
        return NULL;
    }
    
    // Добавляем символ в буфер
    command[command_buf_idx] = symbol;
    command_buf_idx++;
    
    return NULL;
}
