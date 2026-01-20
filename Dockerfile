FROM ubuntu:22.04

# Install build + runtime dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    python3 \
    python3-pip \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy source
COPY . .

# Build the C++ engine (Makefile is in /app/src)
RUN make -C src

# Install python deps (requirements is in /app/lichess-bot)
RUN pip3 install --no-cache-dir -r lichess-bot/requirements.txt

# Put the compiled engine where lichess-bot expects it
RUN mkdir -p lichess-bot/engines \
    && cp src/chess lichess-bot/engines/chess \
    && chmod +x lichess-bot/engines/chess

# Create a minimal Render-friendly config file.
# Token is overridden at runtime by env var LICHESS_BOT_TOKEN (handled in lib/config.py)
RUN cat > lichess-bot/config.render.yml << 'EOF'
token: "RENDER_WILL_OVERRIDE"
url: "https://lichess.org/"

engine:
  dir: "./engines"
  name: "chess"
  protocol: "uci"
  ponder: true

challenge: {}
EOF

WORKDIR /app/lichess-bot

CMD ["python3", "lichess-bot.py", "--config", "config.render.yml", "--disable_auto_logging"]
