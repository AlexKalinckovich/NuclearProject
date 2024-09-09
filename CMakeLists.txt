cmake_minimum_required(VERSION 3.10)

# Название проекта
project(MySFMLProject)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)

# Пути к SFML
set(SFML_DIR "E:\\SFML-2.6.1\\lib\\cmake\\SFML")

# Добавление исполняемого файла
add_executable(MySFMLProject 
    src/main.cpp 
    src/Weapon/Weapon.cpp 
    src/Player/Player.cpp 
    src/Ability/Ability.cpp
    src/Ability/ShieldAbility/ShieldAbility.cpp
    src/Ability/RollAbility/RollAbility.cpp
    src/Bullet/Bullet.cpp
    src/Bullet/BulletPool.cpp
    src/Map/Map.cpp
    src/Enemy/Enemy.cpp
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
)

# Подключение SFML
find_package(SFML 2.6.1 COMPONENTS system window graphics REQUIRED)
target_link_libraries(MySFMLProject sfml-system sfml-window sfml-graphics)
