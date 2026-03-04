#include <stdio.h>
#include "pico/stdlib.h"
#include "main.h"
#include "led/led.h"

int main() {
    // Инициализация stdio (для USB)
    stdio_init_all();
    
    // Инициализация светодиода
    led_init();
    
    // Небольшая задержка для стабилизации USB-соединения
    sleep_ms(2000);
    
    // Вывод приветственного сообщения
    printf("ON-OFF Control Program Started\n");
    printf("Device: %s, Version: %s\n", DEVICE_NAME, DEVICE_VERSION);
    printf("Commands:\n");
    printf("  'e' - Enable LED (turn ON)\n");
    printf("  'd' - Disable LED (turn OFF)\n");
    printf("  'v' - Show version info\n");
    printf("  Any other key - Show error message\n");
    printf("----------------------------------------\n");
    
    // Бесконечный цикл
    while (true) {
        // Считываем символ из терминала
        char symbol = getchar();
        
        // Обработка команды
        switch(symbol) {
            case 'e':
                // Включение светодиода
                led_enable();
                printf("LED turned ON\n");
                break;
                
            case 'd':
                // Выключение светодиода
                led_disable();
                printf("LED turned OFF\n");
                break;
                
            case 'v':
                // Информация о версии прошивки и имени устройства
                printf("Device: %s, Version: %s\n", DEVICE_NAME, DEVICE_VERSION);
                break;
                
            case '\n':
            case '\r':
                // Игнорируем символы перевода строки
                break;
                
            default:
                // Информационное сообщение с неверно введенным символом и его кодом
                printf("Unknown command: '%c' (ASCII: %d, 0x%X)\n", 
                       symbol, symbol, symbol);
                printf("Available commands: 'e' (on), 'd' (off), 'v' (version)\n");
                break;
        }
        
        // Небольшая пауза для удобства чтения
        sleep_ms(10);
    }
    
    return 0;
}
