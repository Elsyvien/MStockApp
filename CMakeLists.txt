cmake_minimum_required(VERSION 3.16)
project(MStockApp VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if Qt is available, but make it optional for now
option(USE_QT "Use Qt GUI (requires Qt5 or Qt6 installation)" OFF)

if(USE_QT)
    # Try to find Qt6 first, then fallback to Qt5
    find_package(Qt6 QUIET COMPONENTS Core Widgets Charts Network)
    if(NOT Qt6_FOUND)
        find_package(Qt5 REQUIRED COMPONENTS Core Widgets Charts Network)
        set(QT_VERSION_MAJOR 5)
    else()
        set(QT_VERSION_MAJOR 6)
    endif()

    # Enable Qt's meta-object system
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    # Source files
    set(SOURCES
        src/main.cpp
        src/ui/mstock.cpp
    )

    # Header files
    set(HEADERS
        src/ui/mstock.h
    )

    # UI files
    set(UI_FILES
        src/ui/mstock.ui
    )

    # Create executable
    add_executable(MStockApp ${SOURCES} ${HEADERS} ${UI_FILES})

    # Link Qt libraries (version-dependent)
    if(QT_VERSION_MAJOR EQUAL 6)
        target_link_libraries(MStockApp
            Qt6::Core
            Qt6::Widgets
            Qt6::Charts
            Qt6::Network
        )
    else()
        target_link_libraries(MStockApp
            Qt5::Core
            Qt5::Widgets
            Qt5::Charts
            Qt5::Network
        )
    endif()

    # Include directories
    target_include_directories(MStockApp PRIVATE src)
    message(STATUS "Building with Qt${QT_VERSION_MAJOR}")

else()
    # Console application without Qt (for testing the build system)
    set(CONSOLE_SOURCES
        src/console_main.cpp
    )

    add_executable(MStockApp ${CONSOLE_SOURCES})
    target_include_directories(MStockApp PRIVATE src)

    message(STATUS "Building console version (Qt not available)")
    message(STATUS "To build with Qt GUI, install Qt and run: cmake -DUSE_QT=ON ..")
    message(STATUS "Install Qt with:")
    message(STATUS "  Ubuntu/Debian: sudo apt install qtbase5-dev qtcharts5-dev")
    message(STATUS "  Fedora: sudo dnf install qt5-qtbase-devel qt5-qtcharts-devel")
    message(STATUS "  Arch: sudo pacman -S qt5-base qt5-charts")
endif()
