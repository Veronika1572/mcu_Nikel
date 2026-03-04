#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Определяем имя устройства и версию
#define DEVICE_NAME "my-pico-device"
#define DEVICE_VERSION "v0.2.0"

// Определяем пин светодиода (на Pico встроенный светодиод на GP25)
#define LED_PIN 25

int main() {
    // Инициализация stdio (для USB)
    stdio_init_all();
    
    // Инициализация пина светодиода
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, false);  // Начинаем с выключенным светодиодом
    
    // Небольшая задержка для стабилизации USB-соединения
    sleep_ms(2000);
    
    // Вывод приветственного сообщения
    printf("Device: %s, Version: %s\n", DEVICE_NAME, DEVICE_VERSION);
    printf("Echo program started. Type any character...\n");
    printf("Special commands:\n");
    printf("  'e' - Enable LED (turn ON)\n");
    printf("  'd' - Disable LED (turn OFF)\n");
    printf("  'v' - Show version info\n");
    printf("----------------------------------------\n");
    
    // Бесконечный цикл
    while (true) {
        // Считываем символ из терминала
        char symbol = getchar();
        
        // Выводим считанный символ и его ASCII-код
        printf("Вы ввели: '%c'\n", symbol);
        printf("ASCII код: %d (десятичный)\n", symbol);
        printf("ASCII код: 0x%X (шестнадцатеричный)\n", symbol);
        
        // Обработка специальных команд
        switch(symbol) {
            case 'e':
                // Включение светодиода
                gpio_put(LED_PIN, true);
                printf("led enable done\n");
                break;
                
            case 'd':
                // Выключение светодиода
                gpio_put(LED_PIN, false);
                printf("led disable done\n");
                break;
                
            case 'v':
                // Вывод имени и версии устройства
                printf("Device: %s\n", DEVICE_NAME);
                printf("Version: %s\n", DEVICE_VERSION);
                break;
                
            case '\n':
            case '\r':
                // Игнорируем символы перевода строки
                // Они уже вывели ASCII коды выше, поэтому просто ничего не делаем
                break;
                
            default:
                // Для всех остальных символов просто выводим информацию
                // которая уже выведена выше
                break;
        }
        
        printf("----------------------------------------\n");
        
        // Небольшая пауза для удобства чтения
        sleep_ms(10);
    }
    
    return 0;
}
