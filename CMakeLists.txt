cmake_minimum_required(VERSION 3.10)

# Название проекта
project(MySFMLProject)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)

# Пути к SFML
set(SFML_DIR "C:/Users/brota/CLionProjects/SFML-2.6.1/lib/cmake/SFML")

# Подключение SFML
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

if(NOT SFML_FOUND)
    message(FATAL_ERROR "SFML not found!")
endif()

# Добавление исполняемого файла
add_executable(MySFMLProject
        src/Weapon/Weapon.cpp
        src/Player/Player.cpp
        src/Ability/Ability.cpp
        src/Ability/ShieldAbility/ShieldAbility.cpp
        src/Ability/RollAbility/RollAbility.cpp
        src/Bullet/Bullet.cpp
        src/Bullet/BulletPool.cpp
        src/Map/Map.cpp
        src/Enemy/Enemy.cpp
        src/main.cpp
        src/Thread/ThreadPool.cpp
        src/Thread/ThreadPool.cpp
        src/Game/Game.cpp
)

# Добавление include директорий
target_include_directories(MySFMLProject PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Weapon
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Player
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Ability
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Ability/ShieldAbility
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Ability/RollAbility
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Bullet
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Map
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Enemy
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Thread
)

# Линковка SFML библиотек
target_link_libraries(MySFMLProject
        sfml-graphics
        sfml-window
        sfml-system
)

# Вывод информации об успехе
message(STATUS "SFML successfully linked.")
