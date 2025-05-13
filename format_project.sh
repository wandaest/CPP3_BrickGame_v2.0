#!/bin/bash

# Проверяем, установлен ли clang-format
if ! command -v clang-format &> /dev/null; then
    echo "clang-format not found. Please install it."
    exit 1
fi

# Находим все C/C++ файлы и применяем clang-format
find . -type f \( -name "*.h" -o -name "*.cc" -o -name "*.cpp" \) -exec clang-format -style=file -i {} \;

echo "All C/C++ files have been formatted according to Google Style."