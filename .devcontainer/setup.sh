#!/usr/bin/env bash
# .devcontainer/setup.sh — runs automatically when the Codespace is created
# Installs libcurl and downloads the single-header JSON library.

set -e

echo "============================================"
echo "  CS 3370 AI Module — Environment Setup"
echo "============================================"

# 1) Install libcurl development headers
echo "[1/3] Installing libcurl..."
sudo apt-get update -qq
sudo apt-get install -y -qq libcurl4-openssl-dev > /dev/null

# 2) Download nlohmann/json single header (if not already present)
if [ ! -f "json.hpp" ]; then
    echo "[2/3] Downloading json.hpp..."
    curl -sL -o json.hpp \
        https://github.com/nlohmann/json/releases/latest/download/json.hpp
    echo "      Downloaded json.hpp ($(wc -c < json.hpp) bytes)"
else
    echo "[2/3] json.hpp already present — skipping."
fi

# 3) Verify everything works
echo "[3/3] Verifying toolchain..."
echo "       g++ version: $(g++ --version | head -1)"
echo "       curl-config: $(curl-config --version)"
echo "       json.hpp:    present"

echo ""
echo "============================================"
echo "  Setup complete!  To build and run:"
echo ""
echo "    make            # compile ai_chat"
echo "    make run        # compile and run"
echo "    make clean      # remove binary"
echo ""
echo "  Don't forget to set your API key first:"
echo "    export ANTHROPIC_API_KEY=\"sk-ant-...\""
echo "============================================"
