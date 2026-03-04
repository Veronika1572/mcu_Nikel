# Требуем минимальную версию cmake
cmake_minimum_required(VERSION 3.15)

# Задаем параметры платы и МК
set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

# Подключаем Pico SDK
include(pico_sdk_import.cmake)

# Создаем проект
project(02-echo)

# Инициализируем SDK
pico_sdk_init()

# Добавляем исполняемый файл
add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Линкуем библиотеки
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_gpio
)

# Устанавливаем скрипт линковки
pico_set_linker_script(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/memmap_rp2040.ld)
pico_enable_stdio_uart(${PROJECT_NAME} 0)
pico_enable_stdio_usb(${PROJECT_NAME} 1)
# Генерируем дополнительные выходные файлы
pico_add_extra_outputs(${PROJECT_NAME})



