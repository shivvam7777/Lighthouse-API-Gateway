#ifndef MIDDLEWARE_HPP
#define MIDDLEWARE_HPP

#include <iostream>
#include <string>

// Abstract Base Class (Interface)
class IMiddleware {
public:
    virtual ~IMiddleware() = default;
    virtual bool execute(const std::string& path) = 0;
};

// 1. Logging Filter
class LoggerMiddleware : public IMiddleware {
public:
    bool execute(const std::string& path) override {
        std::cout << "[LOG]: Incoming request for path: " << path << std::endl;
        return true; 
    }
};

// 2. Auth Filter (Basic Security)
class AuthMiddleware : public IMiddleware {
public:
    bool execute(const std::string& path) override {
        if (path.find("admin") != std::string::npos) {
            std::cout << "[AUTH ERROR]: Unauthorized! Admin access denied." << std::endl;
            return false;
        }
        std::cout << "[AUTH]: User verified successfully." << std::endl;
        return true;
    }
};

#endif