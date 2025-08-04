# Portfolio App - Qt6 C++ Project

A professional desktop application for stock portfolio management built with Qt6 and C++.

## Project Structure

```
PortfolioApp/
├── CMakeLists.txt          # CMake build configuration
├── README.md               # This file
├── src/                    # Source code directory (empty - for you to implement)
├── ui/                     # Qt Designer UI files directory
└── python_scripts/         # Python integration scripts
    └── stock_data.py       # Stock data fetching script (using your yfinance code)
```

## Prerequisites

- Qt6 (with Widgets, Charts, and Network modules)
- CMake 3.16+
- C++17 compatible compiler
- Python 3 with yfinance package (for data fetching)

## Qt Modules Included

- **Qt6::Core** - Core functionality
- **Qt6::Widgets** - GUI widgets
- **Qt6::Charts** - Charts and graphs for financial data
- **Qt6::Network** - Network communication (for API calls)

## CMake Configuration

The CMakeLists.txt is set up with:
- Qt6 auto-MOC, UIC, and RCC enabled
- All necessary Qt modules linked
- Proper include directories configured
- Sections for you to add your source files

## Getting Started

1. **Add your source files** to the `src/` directory
2. **Update CMakeLists.txt** by uncommenting and adding your files to:
   - `SOURCES` list (your .cpp files)
   - `HEADERS` list (your .h files)
   - `UI_FILES` list (your .ui files)
3. **Uncomment the executable and linking sections** in CMakeLists.txt
4. **Build with CMake**

## Python Integration

The `python_scripts/stock_data.py` contains your yfinance code adapted to work with the Qt app:
- Takes stock symbol as command line argument
- Returns JSON data that C++ can easily parse
- Includes historical price data (OHLC + Volume)

## Suggested Classes to Implement

- **MainWindow** - Main application window
- **ChartWidget** - Custom chart component for financial data
- **StockData** - Data management class
- **Portfolio** - Portfolio management logic

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

## Features to Consider

- Real-time stock price updates
- Interactive charts (candlestick, line, volume)
- Portfolio tracking with P&L calculation
- Dark/light themes
- Stock watchlist
- Technical indicators
- Data export functionality

Happy coding! 🚀
