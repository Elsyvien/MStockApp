cmake_minimum_required(VERSION 3.31)
project(MStockApp)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(MStockApp main.cpp MainWindow.cpp)
target_link_libraries(MStockApp
        Qt::Core
        Qt::Gui
        Qt::Widgets
)
