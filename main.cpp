#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include "TrieRouter.hpp"
#include "Middleware.hpp"
#include "RateLimiter.hpp"

// --- New Feature: Round-Robin Load Balancer ---
class LoadBalancer {
private:
    std::vector<std::string> backendServers;
    size_t currentServerIndex;
public:
    LoadBalancer() : currentServerIndex(0) {
        // Real-world distribution ke liye humne backend clusters simulate kiye hain
        backendServers = {"http://srv-prod-01.internal", "http://srv-prod-02.internal", "http://srv-prod-03.internal"};
    }

    std::string getNextServer(const std::string& originalBackend) {
        if (originalBackend == "NOT_FOUND") return originalBackend;
        
        // Base backend route ke aage balance karke server instance load allocate karega
        std::string selectedServer = backendServers[currentServerIndex];
        currentServerIndex = (currentServerIndex + 1) % backendServers.size(); // Round-Robin rotation
        return originalBackend + " -> [Balanced To: " + selectedServer + "]";
    }
};

// --- 1. IP Whitelist Middleware ---
class IPWhitelistMiddleware : public IMiddleware {
private:
    std::set<std::string> allowedIPs = {"127.0.0.1", "192.168.1.1"}; // Sirf ye IPs allowed hain
public:
    bool execute(const std::string& path) override {
        std::string currentIP = "127.0.0.1"; // Real app mein ye request object se aayega
        if (allowedIPs.find(currentIP) != allowedIPs.end()) {
            std::cout << "[IP CHECK]: Access Granted for " << currentIP << std::endl;
            return true;
        }
        std::cout << "[SECURITY]: IP " << currentIP << " is BLOCKED!" << std::endl;
        return false;
    }
};

// --- New Feature: Advanced JWT Authentication Middleware ---
class JWTAuthMiddleware : public IMiddleware {
private:
    std::string mockValidToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9"; // Simulated Valid Token
public:
    bool execute(const std::string& path) override {
        // Real-world scenario: Token HTTP header (Authorization: Bearer <token>) se nikala jata hai
        std::string incomingToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9"; // Testing token

        if (incomingToken.empty()) {
            std::cout << "[AUTH ERROR]: 401 Unauthorized - Missing JWT Token!" << std::endl;
            return false;
        }
        if (incomingToken != mockValidToken) {
            std::cout << "[AUTH ERROR]: 403 Forbidden - Invalid/Expired Token Signature!" << std::endl;
            return false;
        }

        std::cout << "[JWT AUTH]: Token Verified Successfully. User Authorized." << std::endl;
        return true;
    }
};

// --- 2. Response Caching Middleware ---
class CacheMiddleware : public IMiddleware {
private:
    std::map<std::string, std::string> cache; // Path -> Response data
public:
    bool execute(const std::string& path) override {
        if (cache.count(path)) {
            std::cout << "[CACHE HIT]: Returning saved data for " << path << " (Latency: 0ms)" << std::endl;
            return false; // False isliye taaki chain yahi ruk jaye aur backend call na ho
        }
        // Agar cache mein nahi hai toh jaan lijiye data fetch ho gaya aur save ho gaya
        cache[path] = "Success Response Data"; 
        std::cout << "[CACHE MISS]: Data fetched and stored for " << path << std::endl;
        return true;
    }
};

// Config file loader
void loadConfig(TrieRouter& router, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "[ERROR]: Config file missing! Using default mock fallback for testing." << std::endl;
        // Mock fallback agar local system testing ke waqt file nahi milti
        router.addRoute("/api/payments", "PaymentMicroserviceCluster");
        return;
    }
    std::string path, backend;
    while (file >> path >> backend) {
        router.addRoute(path, backend);
        std::cout << "[CONFIG]: Loaded " << path << std::endl;
    }
}

int main() {
    TrieRouter router;
    LoadBalancer balancer;
    
    std::cout << "--- Initializing Gateway v6.0 (Enterprise Auth & Load Balancing) ---" << std::endl;
    loadConfig(router, "../config.txt"); 

    // Chain of Responsibility Setup
    std::vector<std::unique_ptr<IMiddleware>> chain;
    
    // 1. Security Layer (IP Validation)
    chain.push_back(std::make_unique<IPWhitelistMiddleware>());
    
    // 2. Authentication Layer (JWT Validation)
    chain.push_back(std::make_unique<JWTAuthMiddleware>());
    
    // 3. Performance Layer (Caching - 0ms Response Optimization)
    chain.push_back(std::make_unique<CacheMiddleware>());
    
    // 4. Auth & Logging Layer Fallbacks
    chain.push_back(std::make_unique<LoggerMiddleware>());
    chain.push_back(std::make_unique<AuthMiddleware>());

    RateLimiter limiter(3, 10);

    std::cout << "\n--- API Gateway Live ---" << std::endl;

    std::string testPath = "/api/payments";

    // Request Processing Lifecycle - Request 1 (Cache Miss -> Balanced to Backend Instance)
    std::cout << "[REQUEST 1]: Processing standard lifecycle..." << std::endl;
    if (limiter.isAllowed()) {
        bool continueChain = true;
        for (auto& mw : chain) {
            if (!mw->execute(testPath)) { 
                continueChain = false; 
                break; 
            }
        }
        
        if (continueChain) {
            std::string backend = router.getBackend(testPath);
            if(backend != "NOT_FOUND") {
                // Load balancing engine integration
                std::string balancedBackend = balancer.getNextServer(backend);
                std::cout << "[GATEWAY]: Forwarding to -> " << balancedBackend << std::endl;
            } else {
                std::cout << "[404]: Path Not Registered!" << std::endl;
            }
        }
    }

    // [TESTING CACHE HIT] - Wahi request dobara bhejna (Chain should break at CacheMiddleware)
    std::cout << "\n--- Retrying same request (Testing Cache Engine Lifecycle) ---" << std::endl;
    for (auto& mw : chain) {
        if (!mw->execute(testPath)) break; 
    }

    // [TESTING LOAD BALANCER ROTATION] - Agli simulation dikhane ke liye bina cache hit wali dummy request
    std::cout << "\n--- New Request Triggered (Testing Round-Robin Balance Shift) ---" << std::endl;
    std::string backendInstance = router.getBackend(testPath);
    std::cout << "[GATEWAY]: Forwarding to -> " << balancer.getNextServer(backendInstance) << std::endl;

    return 0;
}
