#ifndef RATE_LIMITER_HPP
#define RATE_LIMITER_HPP

#include <iostream>
#include <deque>
#include <chrono>

class RateLimiter {
private:
    int maxRequests;
    int windowInSeconds;
    std::deque<std::chrono::steady_clock::time_point> requests;

public:
    RateLimiter(int limit, int window) : maxRequests(limit), windowInSeconds(window) {}

    bool isAllowed() {
        auto now = std::chrono::steady_clock::now();

        // 1. Purane timestamps remove karo jo window se bahar hain
        while (!requests.empty() && 
               std::chrono::duration_cast<std::chrono::seconds>(now - requests.front()).count() > windowInSeconds) {
            requests.pop_front();
        }

        // 2. Check karo ki limit cross toh nahi hui
        if (requests.size() < static_cast<size_t>(maxRequests)) {
            requests.push_back(now);
            return true;
        }

        // Production-level Alert Log
        std::cout << "[RATE LIMIT ALERT]: 429 Too Many Requests - Throttling active for current window!" << std::endl;
        return false;
    }
};

#endif
