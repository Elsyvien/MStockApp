# python/fetch_stock.py
import sys, json, time
import yfinance as yf

def fetch(prices_for):
    data = {}
    for ticker in prices_for:
        try:
            info = yf.Ticker(ticker).fast_info
            data[ticker] = {
                "last": float(info["last_price"]),
                "currency": info.get("currency", "USD"),
            }
        except Exception as e:
            data[ticker] = {"error": str(e)}
    return data

def main():
    line = sys.stdin.readline()
    req = json.loads(line) # expected: {"tickers":["AAPL","MSFT"]}
    tickers = req.get("tickers", [])
    out = fetch(tickers)
    print(f"Fetched {len(out)}", file=sys.stderr) # For Debugging
    sys.stdout.write(json.dumps({"ts": int(time.time()), "data": out}) + "\n")
    sys.stdout.flush()

if __name__ == "__main__":
    main()
