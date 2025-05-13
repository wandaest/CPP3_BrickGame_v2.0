cmake_minimum_required(VERSION 3.16)

project(BrickGame VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR YES)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Устанавливаем путь к Qt
set(Qt6_DIR "/home/wandaest/Qt/6.8.1/gcc_64/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "/home/wandaest/Qt/6.8.1/gcc_64")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(GTest REQUIRED)

# Поиск библиотеки ncurses
find_library(NCURSES_LIBRARY NAMES ncurses)

# Установка флагов компиляции
if (${TETRIS})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -Werror -Wextra -DTETRIS=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror -Wextra -DTETRIS=1")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -Werror -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror -Wextra")
endif()

# Заголовочные файлы
set(HEADERS
    gui/cli/CliView.h
    gui/cli/CliWrapper.h
    gui/desktop/DesktopGui.h
    controller/Controller.h
    brick_game/tetris/s21_tetris_model.h
    brick_game/tetris/s21_tetris.h
    brick_game/AbstractGameModel/AbstractGameModel.h
    brick_game/CommonGameLogic/CommonGameLogic.h
    brick_game/GameInfo/GameInfo.h
    brick_game/snake/SnakeModel.h
)

# Исходные файлы
set(SOURCES
    desktop_main.cc
    brick_game/snake/SnakeModel.cc
    gui/desktop/DesktopGui.cc
    brick_game/tetris/s21_tetris_model.cc
    brick_game/tetris/s21_tetris.c
    controller/Controller.cc
    gui/cli/CliView.c
    gui/cli/CliWrapper.cc
)

# Тесты
add_executable(snake_test
    tests/SnakeModel_tests.cc
    brick_game/snake/SnakeModel.cc
)
target_include_directories(snake_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/brick_game/snake
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/cli
)
target_link_libraries(snake_test PRIVATE GTest::gtest GTest::gtest_main ${NCURSES_LIBRARY})

# Создание исполняемых файлов
if (${TETRIS})
    if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(tetris_desktop
            MANUAL_FINALIZATION
            ${SOURCES}
            ${HEADERS}
        )
    else()
        add_executable(tetris_desktop
            ${SOURCES}
            ${HEADERS}
        )
    endif()
    target_link_libraries(tetris_desktop PRIVATE Qt6::Widgets ${NCURSES_LIBRARY})
    if(QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(tetris_desktop)
    endif()
    install(TARGETS tetris_desktop
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
else()
    if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        qt_add_executable(snake_desktop
            MANUAL_FINALIZATION
            ${SOURCES}
            ${HEADERS}
        )
    else()
        add_executable(snake_desktop
            ${SOURCES}
            ${HEADERS}
        )
    endif()
    target_link_libraries(snake_desktop PRIVATE Qt6::Widgets ${NCURSES_LIBRARY})
    if(QT_VERSION_MAJOR EQUAL 6)
        qt_finalize_executable(snake_desktop)
    endif()
    install(TARGETS snake_desktop
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

install(TARGETS snake_test
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)