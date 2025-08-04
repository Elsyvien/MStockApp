import finnhub
import yfinance as yf
import matplotlib.pyplot as plt
import numpy as np

finnhub_client = finnhub.Client(api_key="d28j97pr01qmp5u8opo0d28j97pr01qmp5u8opog")

try:
    quote = finnhub_client.quote("NVDA")
    print("Aktueller NVDA Kurs:", quote)
except Exception as e:
    print(f"Fehler beim Abrufen des Kurses: {e}")

nvda = yf.Ticker("NVDA")
hist = nvda.history(period="1y", interval="1d")
print(hist.tail())

hist['SMA_20'] = hist['Close'].rolling(window=20).mean()
hist['SMA_50'] = hist['Close'].rolling(window=50).mean()
hist['RSI'] = 100 - (100 / (1 + hist['Close'].rolling(14).apply(lambda x: x[x > x.shift(1)].sum() / x[x < x.shift(1)].abs().sum())))
hist['Volatility'] = hist['Close'].rolling(window=20).std()

plt.style.use('dark_background')
fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(16, 12))

# Candlestick-style chart
for i in range(len(hist)):
    color = 'lime' if hist['Close'].iloc[i] >= hist['Open'].iloc[i] else 'red'
    ax1.plot([i, i], [hist['Low'].iloc[i], hist['High'].iloc[i]], color=color, linewidth=1)
    ax1.plot([i, i], [hist['Open'].iloc[i], hist['Close'].iloc[i]], color=color, linewidth=3)
ax1.plot(hist['SMA_20'].values, color='orange', linewidth=2, label='SMA 20', alpha=0.8)
ax1.plot(hist['SMA_50'].values, color='cyan', linewidth=2, label='SMA 50', alpha=0.8)
ax1.set_title('NVDA Candlestick Chart with Moving Averages', fontsize=14, fontweight='bold', color='white')
ax1.legend()
ax1.grid(True, alpha=0.3)

# Volume with price overlay
ax2_twin = ax2.twinx()
colors = ['lime' if hist['Close'].iloc[i] >= hist['Open'].iloc[i] else 'red' for i in range(len(hist))]
ax2.bar(range(len(hist)), hist['Volume'], color=colors, alpha=0.6)
ax2_twin.plot(hist['Close'].values, color='yellow', linewidth=2, label='Price')
ax2.set_title('Volume vs Price', fontsize=14, fontweight='bold', color='white')
ax2.set_ylabel('Volume', color='white')
ax2_twin.set_ylabel('Price ($)', color='white')
ax2_twin.legend()
ax2.grid(True, alpha=0.3)

# RSI Oscillator
ax3.plot(hist['RSI'].values, color='magenta', linewidth=2)
ax3.axhline(y=70, color='red', linestyle='--', alpha=0.7, label='Overbought (70)')
ax3.axhline(y=30, color='lime', linestyle='--', alpha=0.7, label='Oversold (30)')
ax3.fill_between(range(len(hist)), hist['RSI'].values, 50, alpha=0.3, color='purple')
ax3.set_title('RSI (Relative Strength Index)', fontsize=14, fontweight='bold', color='white')
ax3.set_ylabel('RSI', color='white')
ax3.set_ylim(0, 100)
ax3.legend()
ax3.grid(True, alpha=0.3)

# Volatility heatmap
volatility_2d = hist['Volatility'].values.reshape(-1, 1)
im = ax4.imshow(volatility_2d.T, cmap='hot', aspect='auto', interpolation='bilinear')
ax4.set_title('Volatility Heatmap', fontsize=14, fontweight='bold', color='white')
ax4.set_ylabel('Volatility Level', color='white')
ax4.set_xlabel('Time Period', color='white')
plt.colorbar(im, ax=ax4, label='Volatility')

plt.tight_layout()
plt.show()



