# python/fetch_stock.py
import sys, json, time
import yfinance as yf
import pandas as pd

def fetch(tickers):
    out = {}
    for t in tickers:
        try:
            fi = yf.Ticker(t).fast_info
            last = fi["last_price"] if isinstance(fi, dict) else getattr(fi, "last_price", None)
            curr = (fi.get("currency") if isinstance(fi, dict) else getattr(fi, "currency", None)) or "USD"
            if last is None:
                out[t] = {"error": "No data available (no last_price)"}
                continue
            out[t] = {"last": float(last), "currency": curr}
        except Exception as e:
            out[t] = {"error": str(e)}
    return out

def fetch_history(ticker, period="1mo", interval="1d", max_points=500):
    try:
        tk = yf.Ticker(ticker)
        hist = tk.history(period=period, interval=interval)
        if hist.empty:
            return {"error": "No data available"}
        hist = hist.dropna(subset=["Close"])

        # DatetimeIndex -> ms since epoch (robust, ohne .asi8/.value)
        idx = hist.index
        if isinstance(idx, pd.DatetimeIndex):
            # Zeitzone bereinigen (UTC) und tz-info entfernen
            if idx.tz is not None:
                idx = idx.tz_convert("UTC").tz_localize(None)
        else:
            # Fallback: was auch immer der Index ist -> DatetimeIndex
            idx = pd.to_datetime(idx, utc=True).tz_convert(None)

        ms = idx.to_numpy(dtype="datetime64[ms]").astype("int64")  # <- hier ist der Fix
        closes = hist["Close"].astype(float).to_numpy()

        pts = [[int(t), float(c)] for t, c in zip(ms, closes)]

        # Downsampling
        if len(pts) > max_points:
            step = len(pts) / max_points
            pts = [pts[int(i * step)] for i in range(max_points)]

        return {"points": pts}
    except Exception as e:
        return {"error": str(e)}

def main():
    line = sys.stdin.readline()
    req = json.loads(line)

    mode = req.get("mode", "prices")
    if mode == "history":
        t = req.get("ticker")
        period = req.get("period", "1mo")
        interval = req.get("interval", "1d")
        payload = {
            "type": "history",
            "ticker": t,
            "series": fetch_history(t, period, interval),
        }
    else:
        tickers = req.get("tickers", [])
        payload = {
            "type": "prices",
            "data": fetch(tickers),
        }

    payload["ts"] = int(time.time())
    print(f"OK {payload['type']}", file=sys.stderr)  # Debug -> stderr
    sys.stdout.write(json.dumps(payload) + "\n")
    sys.stdout.flush()

if __name__ == "__main__":
    main()
