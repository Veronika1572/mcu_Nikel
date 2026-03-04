# on_off.py - Управление светодиодом на Raspberry Pi Pico

import serial
import sys
import time

# === НАСТРОЙКИ ===
# Укажите правильный COM-порт (посмотрите в Диспетчере устройств)
serialName = 'COM14'  # ИЗМЕНИТЕ НА ВАШ ПОРТ!
serialBaudRate = 115200
# =================

def main():
    """Основная функция программы"""
    
    # Вывод о начале работы
    print('=== Управление светодиодом на Pico ===')
    print(f'Подключение к {serialName}...')
    
    # Переменная для хранения объекта порта
    ser = None
    
    try:
        # Открываем COM-порт
        ser = serial.Serial(serialName, baudrate=serialBaudRate, timeout=1)
        print('✅ Подключено успешно!')
        print('\nДоступные команды:')
        print('  e - Включить светодиод')
        print('  d - Выключить светодиод')
        print('  v - Информация о версии')
        print('  q - Выход')
        print('  h - Показать это меню')
        print('─' * 40)
        
        # Основной цикл
        while True:
            # Запрос ввода у пользователя
            command = input('Введите команду (e/d/v/q/h): ').strip().lower()
            
            # Проверка на выход
            if command == 'q':
                print('👋 Завершение работы...')
                break
                
            # Показ меню
            if command == 'h':
                print('\nДоступные команды:')
                print('  e - Включить светодиод')
                print('  d - Выключить светодиод')
                print('  v - Информация о версии')
                print('  q - Выход')
                print('  h - Показать это меню')
                print('─' * 40)
                continue
            
            # Отправка команды на Pico
            if command in ['e', 'd', 'v']:
                # Отправляем команду (обязательно encode в ASCII)
                ser.write(command.encode('ascii'))
                print(f'➡️ Отправлено: "{command}"')
                
                # Небольшая задержка для ответа
                time.sleep(0.1)
                
                # Чтение ответа от Pico
                while ser.in_waiting:
                    response = ser.readline().decode('ascii', errors='ignore').strip()
                    if response:
                        print(f'📟 Ответ Pico: {response}')
            else:
                print('❌ Неверная команда! Введите "h" для списка команд.')
    
    except serial.SerialException as e:
        print(f'❌ Ошибка открытия порта {serialName}: {e}')
        print('Проверьте:')
        print('  - Правильно ли указан COM-порт')
        print('  - Подключено ли устройство')
        print('  - Не занят ли порт другой программой (например, PUTTY)')
        return 1
    
    except KeyboardInterrupt:
        print('\n👋 Программа прервана пользователем')
    
    finally:
        # ВАЖНО: Закрываем порт при выходе
        if ser and ser.is_open:
            ser.close()
            print('🔌 Порт закрыт')
    
    print('✅ Программа завершена')
    return 0

# Точка входа в программу
if __name__ == "__main__":
    sys.exit(main())
