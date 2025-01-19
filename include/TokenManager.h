#ifndef TOKEN_MANAGER_HPP
#define TOKEN_MANAGER_HPP

#include "stable_vector.h"
#include <string>
#include <mutex>
#include <memory>

// Struct to hold authentication details
struct AuthDetails {
    std::string access_token;
    std::string refresh_token;
    int expires_in;
    std::string scope;
};

// TokenManager class for centralized token storage
class TokenManager {
private:
    stable_vector<AuthDetails> tokens;  // Storage for authentication details
    std::mutex mutex;                   // Mutex for thread-safe access

    // Private constructor for Singleton pattern
    TokenManager() = default;

public:
    // Delete copy constructor and assignment operator to prevent multiple instances
    TokenManager(const TokenManager&) = delete;
    TokenManager& operator=(const TokenManager&) = delete;

    // Static method to get the single instance of the class
    static TokenManager& getInstance() {
        static TokenManager instance;
        return instance;
    }

    // Store a new token in the manager
    void storeToken(const AuthDetails& token) {
        std::lock_guard<std::mutex> lock(mutex);
        tokens.push_back(token);
    }

    // Retrieve the first token (if available)
    std::shared_ptr<AuthDetails> getToken() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!tokens.empty()) {
            return std::make_shared<AuthDetails>(tokens[0]);
        }
        return nullptr;
    }
};

#endif // TOKEN_MANAGER_HPP
