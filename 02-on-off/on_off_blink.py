# on_off_blink.py - Управление миганием светодиода на Raspberry Pi Pico

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
    print('=== Управление миганием светодиода на Pico ===')
    print(f'Подключение к {serialName}...')
    
    # Переменная для хранения объекта порта
    ser = None
    
    try:
        # Открываем COM-порт
        ser = serial.Serial(serialName, baudrate=serialBaudRate, timeout=1)
        print('✅ Подключено успешно!')
        print('─' * 50)
        
        # Бесконечный цикл для ввода периодов
        while True:
            try:
                # Инструкция для пользователя
                print('\nВведите период мигания в миллисекундах (целое число)')
                print('Или "q" для выхода, "s" для остановки мигания:')
                
                # Получаем ввод от пользователя
                user_input = input('Период (ms): ').strip().lower()
                
                # Проверка на выход
                if user_input == 'q':
                    print('👋 Завершение работы...')
                    break
                
                # Проверка на остановку мигания
                if user_input == 's':
                    print('⏹️ Останавливаем мигание...')
                    # Отправляем команду выключения светодиода
                    ser.write(b'd')
                    time.sleep(0.1)
                    # Очищаем буфер ответов
                    while ser.in_waiting:
                        ser.readline()
                    continue
                
                # Преобразуем ввод в целое число
                period_ms = int(user_input)
                
                # Проверка на положительное значение
                if period_ms <= 0:
                    print('❌ Период должен быть положительным числом!')
                    continue
                
                print(f'🔄 Начинаем мигание с периодом {period_ms} мс')
                print('Нажмите Ctrl+C для остановки мигания и возврата к вводу периода')
                
                # Цикл мигания
                try:
                    while True:
                        # Включаем светодиод
                        ser.write(b'e')
                        print('  🔴 LED ON')
                        time.sleep(period_ms / 1000.0)  # Переводим мс в секунды
                        
                        # Выключаем светодиод
                        ser.write(b'd')
                        print('  ⚫ LED OFF')
                        time.sleep(period_ms / 1000.0)
                        
                except KeyboardInterrupt:
                    # Пользователь прервал мигание
                    print('\n⏸️ Мигание остановлено')
                    # Убеждаемся, что светодиод выключен
                    ser.write(b'd')
                    # Очищаем буфер
                    while ser.in_waiting:
                        ser.readline()
                    continue
                    
            except ValueError:
                print('❌ Ошибка: введите целое число или "q" для выхода')
            except serial.SerialException as e:
                print(f'❌ Ошибка связи: {e}')
                break
    
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
        # ВАЖНО: Закрываем порт и выключаем светодиод при выходе
        if ser and ser.is_open:
            # Выключаем светодиод перед закрытием
            try:
                ser.write(b'd')
                time.sleep(0.1)
            except:
                pass
            ser.close()
            print('🔌 Порт закрыт')
    
    print('✅ Программа завершена')
    return 0

# Точка входа в программу
if __name__ == "__main__":
    sys.exit(main())
