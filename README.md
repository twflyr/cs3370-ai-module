# CS 3370 — AI Module: Querying Claude from C++

A starter project for building a C++ program that talks to the
[Anthropic (Claude) API](https://docs.anthropic.com) using **libcurl**
and **nlohmann/json**.

---

## Quick Start (GitHub Codespaces)

1. **Open in Codespaces** — click the green **"Code"** button on the repo
   page, then **"Create codespace on main"**.  
   Everything installs automatically (takes ~60 seconds the first time).

2. **Set your API key** — in the Codespace terminal:
   ```bash
   export ANTHROPIC_API_KEY="sk-ant-your-key-here"
   ```
   > Get a free key at <https://console.anthropic.com>

3. **Build and run:**
   ```bash
   make run
   ```
   That's it. Type a question, get a response from Claude.

---

## Quick Start (Local Machine)

### Ubuntu / Debian / WSL
```bash
sudo apt install libcurl4-openssl-dev g++ make
wget -O json.hpp https://github.com/nlohmann/json/releases/latest/download/json.hpp
export ANTHROPIC_API_KEY="sk-ant-your-key-here"
make run
```

### macOS (Homebrew)
```bash
brew install curl nlohmann-json
# json.hpp is installed system-wide; create a local symlink:
ln -s $(brew --prefix)/include/nlohmann/json.hpp json.hpp
export ANTHROPIC_API_KEY="sk-ant-your-key-here"
make run
```

### Windows
Use WSL2 (Ubuntu) and follow the Linux instructions above.
Alternatively, install vcpkg and run `vcpkg install curl nlohmann-json`.

---

## Project Structure

```
cs3370-ai-module/
├── .devcontainer/
│   ├── devcontainer.json   ← Codespaces config (auto-installs libs)
│   └── setup.sh            ← setup script (runs on first open)
├── .vscode/
│   └── tasks.json          ← Ctrl+Shift+B to build
├── ai_chat.cpp             ← ★ the sample program (start here)
├── Makefile                ← type 'make' to compile
├── .gitignore
└── README.md               ← you are here
```

---

## How It Works

The program follows a simple pipeline:

```
User input  →  buildRequestBody()  →  callClaude()  →  extractReply()  →  print
  string           json object         HTTP POST        parsed text       cout
```

| Function            | What it does                                        |
|---------------------|-----------------------------------------------------|
| `WriteCallback`     | libcurl callback — appends each data chunk to a string |
| `buildRequestBody`  | Creates the JSON payload the Claude API expects     |
| `callClaude`        | Configures curl, sends the POST, returns raw JSON   |
| `extractReply`      | Parses the JSON response and extracts the text      |
| `main`              | REPL loop — read input, call Claude, print reply    |

---

## Getting an API Key

1. Go to <https://console.anthropic.com>
2. Sign up (free tier includes $5 of credit)
3. Go to **API Keys** → **Create Key**
4. Copy the key (starts with `sk-ant-...`)
5. Set it as an environment variable — **never paste it into your code**

---

## Exercises

Once you've got the sample running, try these:

### Exercise 1: Code Reviewer (Intermediate)
Read a `.cpp` file with `std::ifstream`, send its contents to Claude
with a system prompt like *"Review this C++ code for bugs and style
issues."*  Display the feedback.

### Exercise 2: Multi-Turn Chat (Advanced)
Keep a `std::vector<json>` of messages. After each exchange, append
both the user message and the assistant reply so Claude remembers
the full conversation history.

### Exercise 3: Quiz Generator (Advanced)
Accept a topic as a command-line argument. Ask Claude to generate 5
multiple-choice questions in JSON format. Parse the structured
response and run an interactive quiz loop in the terminal.

---

## Useful Links

- [Anthropic API docs](https://docs.anthropic.com)
- [libcurl tutorial](https://curl.se/libcurl/c/libcurl-tutorial.html)
- [nlohmann/json documentation](https://json.nlohmann.me)
- [GitHub Codespaces docs](https://docs.github.com/en/codespaces)
