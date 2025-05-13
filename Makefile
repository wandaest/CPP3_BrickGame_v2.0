# Корневая директория проекта
SRC_DIR := $(shell pwd)

# Компиляторы и флаги
CC = gcc
FLAGS = -std=c11 -g -Wall -Werror -Wextra
CC+ = g++
FLAGS_CXX = -std=c++17 -g -Wall -Werror -Wextra
TEST_FLAGS = -lgtest_main -lgtest
GCOV_FLAGS = --coverage

# Пути к исходным файлам
SOURCE_TEST_CPP = $(SRC_DIR)/tests/SnakeModel_tests.cc
LIB_TETRIS_SRC = $(SRC_DIR)/brick_game/tetris/s21_tetris.c
LIB_SNAKE_SRC = $(SRC_DIR)/brick_game/snake/SnakeModel.cc
TETRIS_MODEL_SRC = $(SRC_DIR)/brick_game/tetris/s21_tetris_model.cc
CLI_VIEW_SRC = $(SRC_DIR)/gui/cli/CliView.c
CLI_WRAPPER_SRC = $(SRC_DIR)/gui/cli/CliWrapper.cc
CONTROLLER_SRC = $(SRC_DIR)/controller/Controller.cc
CLI_MAIN_SRC = $(SRC_DIR)/cli_main.cc
DESKTOP_MAIN_SRC = $(SRC_DIR)/desktop_main.cc
DESKTOP_GUI_SRC = $(SRC_DIR)/gui/desktop/DesktopGui.cc
DOC_SRC = $(DOC_DIR)/doc.texi

# Имена библиотек и исполняемых файлов
LIB_TETRIS = Tetris
LIB_SNAKE = SnakeModel
TEST = snake_test
SNAKE_CLI = snake_cli
TETRIS_CLI = tetris_cli
SNAKE_DESKTOP = snake_desktop
TETRIS_DESKTOP = tetris_desktop

# Директории
LIB_DIR = lib
BUILD_DIR = ../build
SCORE_DIR = game_score
DIST_DIR = dist
TESTS_DIR = tests
GCOV_REPORT_DIR = report
DOC_DIR = doc
DOC_HTML_DIR = doc_html

# Определение системы
SYSTEM := $(shell uname -s)

ifeq ($(SYSTEM), Linux)
    OPEN_CMD = xdg-open
    ADD_LIB = -lncurses -lsubunit -lm
else ifeq ($(SYSTEM), Darwin)
    OPEN_CMD = open
    ADD_LIB = -lgtest -lncurses
    BREW := ${HOME}/homebrew
else
    $(error Unsupported system: $(SYSTEM))
endif

# Основные цели
.PHONY: all install snake_cli tetris_cli snake_desktop tetris_desktop test clean uninstall dvi dist gcov_report

all: install dvi dist

install: snake_cli tetris_cli snake_desktop tetris_desktop

test: $(BUILD_DIR)/$(TEST)

# Компиляция тестов
$(BUILD_DIR)/$(TEST): $(LIB_DIR)/$(LIB_SNAKE).a $(SOURCE_TEST_CPP) | $(BUILD_DIR)
	$(CC+) $(FLAGS_CXX) $(SOURCE_TEST_CPP) $(LIB_DIR)/$(LIB_SNAKE).a $(TEST_FLAGS) $(ADD_LIB) -o $(BUILD_DIR)/$(TEST)

# Компиляция CLI-версий
snake_cli: $(BUILD_DIR)/$(SNAKE_CLI)

tetris_cli: $(BUILD_DIR)/$(TETRIS_CLI)

$(LIB_DIR)/s21_tetris.o: $(LIB_TETRIS_SRC) $(LIB_DIR)
	$(CC) $(FLAGS) -c $(LIB_TETRIS_SRC) -o $(LIB_DIR)/s21_tetris.o

$(BUILD_DIR)/$(SNAKE_CLI): $(LIB_DIR)/$(LIB_SNAKE).a $(LIB_DIR)/s21_tetris.o $(CLI_MAIN_SRC) $(CLI_VIEW_SRC) $(CLI_WRAPPER_SRC) $(CONTROLLER_SRC) | $(BUILD_DIR) $(SCORE_DIR)
	$(CC) $(FLAGS) -c $(CLI_VIEW_SRC) -o $(LIB_DIR)/CliView.o $(ADD_LIB)
	$(CC+) $(FLAGS_CXX) -c $(CLI_MAIN_SRC) -o $(LIB_DIR)/cli_main.o
	$(CC+) $(FLAGS_CXX) -c $(CLI_WRAPPER_SRC) -o $(LIB_DIR)/CliWrapper.o
	$(CC+) $(FLAGS_CXX) -c $(CONTROLLER_SRC) -o $(LIB_DIR)/Controller.o
	$(CC+) $(FLAGS_CXX) $(LIB_DIR)/cli_main.o $(LIB_DIR)/CliView.o $(LIB_DIR)/CliWrapper.o $(LIB_DIR)/Controller.o $(LIB_DIR)/s21_tetris.o $(LIB_DIR)/$(LIB_SNAKE).a $(ADD_LIB) -o $(BUILD_DIR)/$(SNAKE_CLI)
	rm -rf $(LIB_DIR)/*.o

$(BUILD_DIR)/$(TETRIS_CLI): $(LIB_DIR)/$(LIB_TETRIS).a $(CLI_MAIN_SRC) $(CLI_VIEW_SRC) $(CLI_WRAPPER_SRC) $(CONTROLLER_SRC) $(TETRIS_MODEL_SRC) | $(BUILD_DIR) $(SCORE_DIR)
	$(CC) $(FLAGS) -c $(CLI_VIEW_SRC) -o $(LIB_DIR)/CliView.o -DTETRIS=1 $(ADD_LIB)
	$(CC+) $(FLAGS_CXX) -c $(CLI_MAIN_SRC) -o $(LIB_DIR)/cli_main.o -DTETRIS=1
	$(CC+) $(FLAGS_CXX) -c $(CLI_WRAPPER_SRC) -o $(LIB_DIR)/CliWrapper.o -DTETRIS=1
	$(CC+) $(FLAGS_CXX) -c $(CONTROLLER_SRC) -o $(LIB_DIR)/Controller.o -DTETRIS=1
	$(CC+) $(FLAGS_CXX) -c $(TETRIS_MODEL_SRC) -o $(LIB_DIR)/TetrisModel.o -DTETRIS=1
	$(CC+) $(FLAGS_CXX) $(LIB_DIR)/cli_main.o $(LIB_DIR)/CliView.o $(LIB_DIR)/CliWrapper.o $(LIB_DIR)/Controller.o $(LIB_DIR)/TetrisModel.o $(LIB_DIR)/$(LIB_TETRIS).a $(ADD_LIB) -o $(BUILD_DIR)/$(TETRIS_CLI) -DTETRIS=1
	rm -rf $(LIB_DIR)/*.o

# Компиляция библиотек
$(LIB_DIR)/$(LIB_TETRIS).a: $(LIB_TETRIS_SRC) $(LIB_DIR)
	$(CC) $(FLAGS) -c $(LIB_TETRIS_SRC) -o $(LIB_DIR)/$(LIB_TETRIS).o -DTETRIS=1
	ar rc $(LIB_DIR)/$(LIB_TETRIS).a $(LIB_DIR)/$(LIB_TETRIS).o
	ranlib $(LIB_DIR)/$(LIB_TETRIS).a
	rm -rf $(LIB_DIR)/*.o

# Явное правило для объектного файла
$(LIB_DIR)/SnakeModel.o: $(LIB_SNAKE_SRC) $(LIB_DIR)
	$(CC+) $(FLAGS_CXX) -c $(LIB_SNAKE_SRC) -o $(LIB_DIR)/SnakeModel.o

# Библиотека зависит от объектного файла
$(LIB_DIR)/$(LIB_SNAKE).a: $(LIB_DIR)/SnakeModel.o
	ar rc $(LIB_DIR)/$(LIB_SNAKE).a $(LIB_DIR)/SnakeModel.o
	ranlib $(LIB_DIR)/$(LIB_SNAKE).a
	rm -rf $(LIB_DIR)/*.o

# Компиляция тестов с покрытием
$(LIB_DIR)/SnakeModel_gcov.o: $(LIB_SNAKE_SRC) $(LIB_DIR)
	$(CC+) $(FLAGS_CXX) $(GCOV_FLAGS) -c $(LIB_SNAKE_SRC) -o $(LIB_DIR)/SnakeModel_gcov.o

$(LIB_DIR)/$(LIB_SNAKE)_gcov.a: $(LIB_DIR)/SnakeModel_gcov.o
	ar rc $(LIB_DIR)/$(LIB_SNAKE)_gcov.a $(LIB_DIR)/SnakeModel_gcov.o
	ranlib $(LIB_DIR)/$(LIB_SNAKE)_gcov.a

$(BUILD_DIR)/$(TEST)_gcov: $(LIB_DIR)/$(LIB_SNAKE)_gcov.a $(SOURCE_TEST_CPP) | $(BUILD_DIR)
	$(CC+) $(FLAGS_CXX) $(GCOV_FLAGS) $(SOURCE_TEST_CPP) $(LIB_DIR)/$(LIB_SNAKE)_gcov.a $(TEST_FLAGS) $(ADD_LIB) -o $(BUILD_DIR)/$(TEST)_gcov

# Генерация отчёта покрытия
gcov_report: $(BUILD_DIR)/$(TEST)_gcov
	$(BUILD_DIR)/$(TEST)_gcov
	lcov --capture --directory $(LIB_DIR) --output-file $(LIB_DIR)/coverage.info --ignore-errors inconsistent
	lcov --remove $(LIB_DIR)/coverage.info '/usr/*' '*/tests/*' --output-file $(LIB_DIR)/coverage_filtered.info
	genhtml $(LIB_DIR)/coverage_filtered.info --output-directory $(GCOV_REPORT_DIR)
	$(OPEN_CMD) $(GCOV_REPORT_DIR)/index.html

# Компиляция десктопных версий
.PHONY: snake_desktop tetris_desktop

snake_desktop:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ../src -DCMAKE_CXX_FLAGS="$(FLAGS_CXX) $(ADD_LIB)" && make snake_desktop

tetris_desktop:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ../src -DTETRIS=1 -DCMAKE_CXX_FLAGS="$(FLAGS_CXX) $(ADD_LIB)" && make tetris_desktop

# Создание директорий
$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(SCORE_DIR):
	mkdir -p $(SCORE_DIR)

dvi:
	makeinfo --html $(SRC_DIR)/doc/doc.texi
	open doc_html/index*

# Создание дистрибутива
dist: | $(DIST_DIR)
	mkdir -p $(DIST_DIR)
	cp -rf brick_game/ $(DIST_DIR)/
	cp -rf gui/ $(DIST_DIR)/
	cp -rf controller/ $(DIST_DIR)/
	cp -rf doc/snake_fsm_diagram.png $(DIST_DIR)/
	cp -rf doc/tetris_fsm_diagram.png $(DIST_DIR)/
	cp -rf Makefile $(DIST_DIR)/
	tar -czf BrickGame_v2.0.tar.gz $(DIST_DIR)/
	rm -rf $(DIST_DIR)/*
	mv BrickGame_v2.0.tar.gz $(DIST_DIR)/

# Очистка и удаление
clean:
	rm -rf $(LIB_DIR) $(BUILD_DIR)/*.o $(BUILD_DIR)/*.a $(GCOV_REPORT_DIR) *.gcno *.gcda $(LIB_DIR)/*.gcno $(LIB_DIR)/*.gcda $(LIB_DIR)/*.info $(BUILD_DIR)/$(DOC_HTML_DIR)

uninstall: clean
	rm -rf $(BUILD_DIR) $(SCORE_DIR) $(DOCS_DIR)/latex $(DOCS_DIR)/html $(DOCS_DIR)/*.dvi $(DIST_DIR)