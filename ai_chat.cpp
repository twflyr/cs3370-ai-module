// ai_chat.cpp — A minimal C++ client for the Anthropic (Claude) API
// ================================================================
// Compile:  g++ -std=c++17 -o ai_chat ai_chat.cpp -lcurl
//    -or-:  make
// Run:      export ANTHROPIC_API_KEY="sk-ant-your-key-here"
//           ./ai_chat
// ================================================================

#include <iostream>
#include <string>
#include <cstdlib>          // getenv()
#include <curl/curl.h>      // libcurl  (link with -lcurl)
#include "json.hpp"         // nlohmann/json — single header

using json = nlohmann::json;

// ----------------------------------------------------------------
// 1) Write callback — libcurl calls this for each chunk of data
//    received from the server.  We simply append it to a string.
// ----------------------------------------------------------------
size_t WriteCallback(void* contents, size_t size,
                     size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;          // must return # bytes handled
}

// ----------------------------------------------------------------
// 2) Build the JSON request body the Claude API expects.
//    See: https://docs.anthropic.com/en/api/messages
// ----------------------------------------------------------------
json buildRequestBody(const std::string& userMessage) {
    return {
        {"model",      "claude-sonnet-4-20250514"},   // fast & capable
        {"max_tokens",  1024},
        {"messages",    json::array({
            {{"role", "user"}, {"content", userMessage}}
        })}
    };
}

// ----------------------------------------------------------------
// 3) Send the request to the Anthropic API using libcurl.
//    Returns the raw JSON response string.
// ----------------------------------------------------------------
std::string callClaude(const std::string& apiKey,
                       const json& requestBody) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        return R"({"error":{"type":"local","message":"curl_easy_init failed"}})";
    }

    // --- Set HTTP headers ---
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers,
        ("x-api-key: " + apiKey).c_str());
    headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");

    // --- Serialize JSON to a string ---
    std::string bodyStr = requestBody.dump();

    // --- Configure curl options ---
    curl_easy_setopt(curl, CURLOPT_URL,
        "https://api.anthropic.com/v1/messages");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);  // 30-second timeout

    // --- Execute the request ---
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        // Build a fake JSON error so the parser handles it uniformly
        response = R"({"error":{"type":"network","message":")" +
                   std::string(curl_easy_strerror(res)) + R"("}})";
    }

    // --- Cleanup ---
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

// ----------------------------------------------------------------
// 4) Parse the API response and extract the assistant's text.
//    Handles both success and error cases.
// ----------------------------------------------------------------
std::string extractReply(const std::string& rawJSON) {
    try {
        json resp = json::parse(rawJSON);

        // Check for API-level errors (auth, rate limit, bad request, etc.)
        if (resp.contains("error")) {
            std::string type = resp["error"].value("type", "unknown");
            std::string msg  = resp["error"].value("message", "no details");
            return "[API Error - " + type + "] " + msg;
        }

        // Happy path: extract the assistant's text content
        if (resp.contains("content") && !resp["content"].empty()) {
            return resp["content"][0]["text"].get<std::string>();
        }

        return "[Error] Response contained no content.";

    } catch (const json::parse_error& e) {
        return std::string("[JSON Parse Error] ") + e.what();
    } catch (const std::exception& e) {
        return std::string("[Unexpected Error] ") + e.what();
    }
}

// ----------------------------------------------------------------
// 5) main — simple REPL loop
// ----------------------------------------------------------------
int main() {
    // Read API key from environment (never hard-code it!)
    const char* envKey = std::getenv("ANTHROPIC_API_KEY");
    if (!envKey) {
        std::cerr << "ERROR: Set the ANTHROPIC_API_KEY environment variable first.\n"
                  << "       export ANTHROPIC_API_KEY=\"sk-ant-your-key-here\"\n";
        return 1;
    }
    std::string apiKey(envKey);

    std::cout << "========================================\n"
              << "   C++ Claude Chat  (type 'quit' to exit)\n"
              << "========================================\n\n";

    std::string userInput;
    while (true) {
        std::cout << "You: ";
        if (!std::getline(std::cin, userInput)) break;   // handle EOF
        if (userInput.empty()) continue;                  // skip blanks
        if (userInput == "quit" || userInput == "exit") break;

        // Build → Send → Parse → Display
        json    body    = buildRequestBody(userInput);
        std::string raw = callClaude(apiKey, body);
        std::string reply = extractReply(raw);

        std::cout << "\nClaude: " << reply << "\n\n";
    }

    std::cout << "Goodbye!\n";
    return 0;
}
