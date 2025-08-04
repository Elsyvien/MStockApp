#!/usr/bin/env python3
import sys
import yfinance as yf
import json
from datetime import datetime

def get_stock_data(symbol):
    """
    Fetch stock data using yfinance and return as JSON
    This is your existing Python code adapted for the Qt app
    """
    try:
        ticker = yf.Ticker(symbol)
        hist = ticker.history(period="1y")
        
        # Convert to format that C++ can easily parse
        data = {
            "symbol": symbol,
            "dates": [],
            "opens": [],
            "highs": [],
            "lows": [],
            "closes": [],
            "volumes": []
        }
        
        for date, row in hist.iterrows():
            data["dates"].append(date.isoformat())
            data["opens"].append(float(row['Open']))
            data["highs"].append(float(row['High']))
            data["lows"].append(float(row['Low']))
            data["closes"].append(float(row['Close']))
            data["volumes"].append(int(row['Volume']))
        
        return json.dumps(data)
        
    except Exception as e:
        error_data = {"error": str(e)}
        return json.dumps(error_data)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(json.dumps({"error": "Usage: python stock_data.py <symbol>"}))
        sys.exit(1)
    
    symbol = sys.argv[1].upper()
    result = get_stock_data(symbol)
    print(result)
