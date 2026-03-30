#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "TrieRouter.hpp"
#include "Middleware.hpp"
#include "RateLimiter.hpp"

// Config file loader
void loadConfig(TrieRouter& router, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "[ERROR]: Config file missing!" << std::endl;
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
    
    // File se routes load karein
    std::cout << "--- Initializing Gateway Configuration ---" << std::endl;
    loadConfig(router, "../config.txt"); 

    std::vector<std::unique_ptr<IMiddleware>> chain;
    chain.push_back(std::make_unique<LoggerMiddleware>());
    chain.push_back(std::make_unique<AuthMiddleware>());

    RateLimiter limiter(3, 10);

    std::cout << "\n--- Custom API Gateway v4.0 (Dynamic) ---" << std::endl;

    // Test a Dynamic Route from File
    std::string testPath = "/api/payments";
    if (limiter.isAllowed()) {
        bool allowed = true;
        for (auto& mw : chain) {
            if (!mw->execute(testPath)) { allowed = false; break; }
        }
        if (allowed) {
            std::cout << "[GATEWAY]: Route found -> " << router.getBackend(testPath) << std::endl;
        }
    }
// --- TEST 4: Unknown Path (The 404 Scenario) ---
    std::cout << "\n[TEST 4] Requesting unknown path: /api/secret_data" << std::endl;
    std::string unknownPath = "/api/secret_data";
    
    std::string result = router.getBackend(unknownPath);
    if (result == "NOT_FOUND") {
        std::cout << "[ERROR 404]: Resource Not Found! The path '" << unknownPath << "' is not registered." << std::endl;
    } else {
        std::cout << "[GATEWAY]: Forwarding to -> " << result << std::endl;
    }
    return 0;
}