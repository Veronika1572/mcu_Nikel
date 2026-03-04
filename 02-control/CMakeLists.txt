# Требуем минимальную версию cmake
cmake_minimum_required(VERSION 3.15)

# Задаем параметры платы и МК
set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

# Подключаем Pico SDK
include(pico_sdk_import.cmake)

# Создаем проект
project(02_control)

# Инициализируем SDK
pico_sdk_init()

# Добавляем исполняемый файл и ВСЕ исходные файлы
add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stdio-task/stdio-task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol-task/protocol-task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/led-task/led-task.c    # ДОБАВЛЕН НОВЫЙ ФАЙЛ
)

# Линкуем библиотеки
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_gpio
)

# Настройка stdio (отключаем UART, включаем USB)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Генерируем дополнительные выходные файлы
pico_add_extra_outputs(${PROJECT_NAME})
