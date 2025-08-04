cmake_minimum_required(VERSION 3.16)
project(PortfolioApp VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts Network)

# Enable Qt's meta-object system
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files (add your .cpp files here)
set(SOURCES
    # src/main.cpp
    # Add your source files here
)

# Header files (add your .h files here)
set(HEADERS
    # src/mainwindow.h
    # Add your header files here
)

# UI files (add your .ui files here)
set(UI_FILES
    # ui/mainwindow.ui
    # Add your UI files here
)

# Create executable (uncomment when you add source files)
# add_executable(PortfolioApp ${SOURCES} ${HEADERS} ${UI_FILES})

# Link Qt libraries (uncomment when you add executable)
# target_link_libraries(PortfolioApp
#     Qt6::Core
#     Qt6::Widgets
#     Qt6::Charts
#     Qt6::Network
# )

# Include directories (uncomment when you add executable)
# target_include_directories(PortfolioApp PRIVATE src)
