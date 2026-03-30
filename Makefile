# Makefile for CS 3370 AI Module
# Usage:
#   make          — compile ai_chat
#   make run      — compile and run
#   make clean    — remove binary

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS  = -lcurl

TARGET   = ai_chat
SRC      = ai_chat.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC) json.hpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

run: $(TARGET)
	@echo ""
	@if [ -z "$$ANTHROPIC_API_KEY" ]; then \
		echo "ERROR: Set your API key first:"; \
		echo "  export ANTHROPIC_API_KEY=\"sk-ant-...\""; \
		echo ""; \
		exit 1; \
	fi
	./$(TARGET)

clean:
	rm -f $(TARGET)
