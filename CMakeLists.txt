cmake_minimum_required(VERSION 3.24)
project(MStockApp)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(MStockApp
        src/main.cpp
        ui/MainWindow.cpp
        src/PythonBridge.h
        ui/MainWindow.h
)

target_link_libraries(MStockApp PRIVATE Qt::Core Qt::Gui Qt::Widgets)
target_include_directories(MStockApp PRIVATE ui src python)

# Copy Python script to the build directory after building
# This assumes the script is located at python/fetch_stock.py relative to the source directory
set(PY_SCRIPT ${CMAKE_SOURCE_DIR}/python/fetch_stock.py)
add_custom_command(TARGET MStockApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PY_SCRIPT} $<TARGET_FILE_DIR:MStockApp>/fetch_stock.py)

# Define the Python executable and script name as preprocessor definitions
target_compile_definitions(MStockApp PRIVATE
        PYTHON_EXE="/usr/bin/python3"
        SCRIPT_NAME="fetch_stock.py"
)
