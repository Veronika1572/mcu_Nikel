#include <stdio.h>
#include "pico/stdlib.h"

// Определяем имя устройства
#define DEVICE_NAME "my-pico-device"

// Определяем версию устройства
#define DEVICE_VRSN "v0.0.1"

int main() {
    // Инициализация stdio (для USB или UART)
    stdio_init_all();
    
    // Небольшая задержка для стабилизации USB-соединения
    sleep_ms(2000);
    
    // Бесконечный цикл
    while (true) {
        // Вывод сообщений в терминал
        printf("Hello World!\n");
        printf("Device name: '%s'\n", DEVICE_NAME);
        printf("Device version: %s\n", DEVICE_VRSN);
        printf("------------------------\n");
        
        // Пауза 1 секунда
        sleep_ms(1000);
    }
    
    return 0;
}
