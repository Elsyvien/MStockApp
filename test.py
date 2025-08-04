import finnhub

finnhub_client = finnhub.Client(api_key="d28j97pr01qmp5u8opo0d28j97pr01qmp5u8opog")

res = finnhub_client.stock_candles("AAPL", "D", 1614556800, 1617235200 )
print(res)