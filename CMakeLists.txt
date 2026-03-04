# Требуем минимальную версию cmake
cmake_minimum_required(VERSION 3.15)

# Задаем параметры платы и МК
set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

# Подключаем Pico SDK
include(pico_sdk_import.cmake)

# Создаем проект
project(01_blink)

# Инициализируем SDK
pico_sdk_init()

# Добавляем исполняемый файл
add_executable(01_blink
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Линкуем библиотеки
target_link_libraries(01_blink
    pico_stdlib
    hardware_gpio
)

# Устанавливаем скрипт линковки
pico_set_linker_script(01_blink ${CMAKE_CURRENT_SOURCE_DIR}/memmap_rp2040.ld)

# Генерируем дополнительные выходные файлы
pico_add_extra_outputs(01_blink)

