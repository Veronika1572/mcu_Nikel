#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Скрипт для сбора данных с Raspberry Pi Pico (проект 03-adc)
Собирает напряжение на GPIO 26 и температуру чипа, строит графики
"""

import time
import serial
import matplotlib.pyplot as plt
import sys

def read_value(ser):
    """
    Читает строку из последовательного порта и преобразует её в float
    Игнорирует ошибки преобразования (например, пустые строки или служебные сообщения)
    """
    while True:
        try:
            # Читаем строку, декодируем из ASCII
            line = ser.readline().decode('ascii').strip()
            
            # Пропускаем пустые строки
            if not line:
                continue
                
            # Пробуем преобразовать в float
            value = float(line)
            return value
        except ValueError:
            # Если не удалось преобразовать, выводим сообщение и продолжаем
            print(f"  (ignored: '{line}')", file=sys.stderr)
            continue
        except UnicodeDecodeError:
            # Ошибка декодирования - просто пропускаем
            continue

def main():
    """Основная функция программы"""
    
    # === НАСТРОЙКИ ===
    # Укажите ваш COM-порт (посмотрите в Диспетчере устройств)
    PORT = 'COM14'  # ИЗМЕНИТЕ НА ВАШ ПОРТ!
    BAUDRATE = 115200
    MEASUREMENT_INTERVAL = 0.1  # Интервал между измерениями в секундах
    # =================
    
    print("=" * 60)
    print("Скрипт сбора данных с Raspberry Pi Pico (03-adc)")
    print("=" * 60)
    print(f"Порт: {PORT}, скорость: {BAUDRATE}")
    print(f"Интервал измерений: {MEASUREMENT_INTERVAL} с")
    print("Команды устройству: get_adc, get_temp")
    print("-" * 60)
    print("Для остановки нажмите Ctrl+C")
    print("-" * 60)
    
    # Создаем объект для работы с последовательным портом
    # timeout=0 - неблокирующий режим
    ser = serial.Serial(port=PORT, baudrate=BAUDRATE, timeout=0.0)
    
    # Проверяем, открылся ли порт
    if ser.is_open:
        print(f"✅ Порт {ser.name} открыт")
    else:
        print(f"❌ Порт {ser.name} не открыт")
        return
    
    # Создаем списки для хранения результатов
    measure_ts = []           # Метки времени
    measure_voltage_V = []    # Напряжение в вольтах
    measure_temperature_C = [] # Температура в градусах Цельсия
    
    # Фиксируем время старта измерений
    start_ts = time.time()
    
    # Счетчик измерений
    measurement_count = 0
    
    try:
        # Бесконечный цикл сбора данных
        while True:
            # Фиксируем текущее время относительно старта
            ts = time.time() - start_ts
            
            # ===== ИЗМЕРЕНИЕ НАПРЯЖЕНИЯ =====
            # Отправляем команду get_adc
            ser.write("get_adc\n".encode('ascii'))
            # Читаем ответ (напряжение в вольтах)
            voltage_V = read_value(ser)
            
            # ===== ИЗМЕРЕНИЕ ТЕМПЕРАТУРЫ =====
            # Отправляем команду get_temp
            ser.write("get_temp\n".encode('ascii'))
            # Читаем ответ (температура в градусах)
            temp_C = read_value(ser)
            
            # Сохраняем результаты
            measure_ts.append(ts)
            measure_voltage_V.append(voltage_V)
            measure_temperature_C.append(temp_C)
            
            # Увеличиваем счетчик
            measurement_count += 1
            
            # Выводим результаты в терминал
            print(f"[{measurement_count:3d}] t={ts:6.2f}с | "
                  f"U={voltage_V:6.3f} В | "
                  f"T={temp_C:6.2f} °C")
            
            # Задержка перед следующим измерением
            time.sleep(MEASUREMENT_INTERVAL)
            
    except KeyboardInterrupt:
        # Пользователь нажал Ctrl+C
        print("\n" + "-" * 60)
        print("⏹️ Сбор данных остановлен пользователем")
        
    except serial.SerialException as e:
        print(f"❌ Ошибка последовательного порта: {e}")
        
    finally:
        # Этот блок выполняется всегда, даже при ошибках
        print(f"📊 Всего измерений: {measurement_count}")
        print("🔄 Закрытие порта...")
        ser.close()
        print("✅ Порт закрыт")
        
        # Если есть данные, строим графики
        if measurement_count > 0:
            print("📈 Построение графиков...")
            plot_measurements(measure_ts, measure_voltage_V, measure_temperature_C)
        else:
            print("❌ Нет данных для построения графиков")

def plot_measurements(timestamps, voltages, temperatures):
    """
    Строит графики напряжения и температуры от времени
    """
    # Создаем фигуру с двумя подграфиками (2 ряда, 1 колонка)
    plt.figure(figsize=(10, 8))
    
    # Верхний график - напряжение
    plt.subplot(2, 1, 1)
    plt.plot(timestamps, voltages, 'b-', linewidth=1.5, label='Напряжение')
    plt.title('График зависимости напряжения от времени', fontsize=14)
    plt.xlabel('время, с', fontsize=12)
    plt.ylabel('напряжение, В', fontsize=12)
    plt.grid(True, alpha=0.3)
    plt.legend()
    
    # Добавляем статистику на график напряжения
    if voltages:
        v_min = min(voltages)
        v_max = max(voltages)
        v_avg = sum(voltages) / len(voltages)
        plt.text(0.02, 0.98, 
                 f'Мин: {v_min:.3f} В\nМакс: {v_max:.3f} В\nСред: {v_avg:.3f} В',
                 transform=plt.gca().transAxes,
                 verticalalignment='top',
                 bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
    
    # Нижний график - температура
    plt.subplot(2, 1, 2)
    plt.plot(timestamps, temperatures, 'r-', linewidth=1.5, label='Температура')
    plt.title('График зависимости температуры от времени', fontsize=14)
    plt.xlabel('время, с', fontsize=12)
    plt.ylabel('температура, °C', fontsize=12)
    plt.grid(True, alpha=0.3)
    plt.legend()
    
    # Добавляем статистику на график температуры
    if temperatures:
        t_min = min(temperatures)
        t_max = max(temperatures)
        t_avg = sum(temperatures) / len(temperatures)
        plt.text(0.02, 0.98,
                 f'Мин: {t_min:.1f} °C\nМакс: {t_max:.1f} °C\nСред: {t_avg:.1f} °C',
                 transform=plt.gca().transAxes,
                 verticalalignment='top',
                 bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
    
    # Настраиваем отступы между графиками
    plt.tight_layout()
    
    # Показываем графики
    print("📊 Отображение графиков...")
    plt.show()
    
    # Спрашиваем, хочет ли пользователь сохранить данные
    save_choice = input("\n💾 Сохранить данные в файл? (y/n): ").strip().lower()
    if save_choice == 'y':
        save_data(timestamps, voltages, temperatures)

def save_data(timestamps, voltages, temperatures):
    """
    Сохраняет данные в CSV файл
    """
    import csv
    from datetime import datetime
    
    # Генерируем имя файла с текущей датой и временем
    filename = f"adc_data_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
    
    try:
        with open(filename, 'w', newline='', encoding='utf-8') as csvfile:
            writer = csv.writer(csvfile)
            # Заголовки
            writer.writerow(['Время (с)', 'Напряжение (В)', 'Температура (°C)'])
            # Данные
            for t, v, temp in zip(timestamps, voltages, temperatures):
                writer.writerow([f"{t:.3f}", f"{v:.3f}", f"{temp:.2f}"])
        
        print(f"✅ Данные сохранены в файл: {filename}")
        
    except Exception as e:
        print(f"❌ Ошибка при сохранении файла: {e}")

# Точка входа в программу
if __name__ == "__main__":
    main()
