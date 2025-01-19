#include "stable_vector.h"
#include <mutex>
#include <memory>

struct AuthDetails {
    std::string access_token;
    std::string refresh_token;
    int expires_in;
    std::string scope;
};

class TokenManager {
private:
    stable_vector<AuthDetails> tokens;
    std::mutex mutex;

    TokenManager() = default; // Private constructor for Singleton pattern

public:
    // Disable copy/move
    TokenManager(const TokenManager&) = delete;
    TokenManager& operator=(const TokenManager&) = delete;

    static TokenManager& getInstance() {
        static TokenManager instance; // Thread-safe initialization in C++11 and later
        return instance;
    }

    void storeToken(const AuthDetails& token) {
        std::lock_guard<std::mutex> lock(mutex);
        tokens.push_back(token);
    }

    std::shared_ptr<AuthDetails> getToken() {
        std::lock_guard<std::mutex> lock(mutex);
        return tokens.empty() ? nullptr : std::make_shared<AuthDetails>(tokens[0]);
    }
};
