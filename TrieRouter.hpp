#ifndef TRIE_ROUTER_HPP
#define TRIE_ROUTER_HPP

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

struct TrieNode {
    std::unordered_map<std::string, std::shared_ptr<TrieNode>> children;
    std::string backendUrl;
    bool isEndOfPath = false;
};

class TrieRouter {
private:
    std::shared_ptr<TrieNode> root;

    std::vector<std::string> splitPath(std::string path) {
        std::vector<std::string> segments;
        if (path.empty() || path == "/") return segments;

        size_t start = (path[0] == '/') ? 1 : 0;
        size_t end = 0;
        while ((end = path.find('/', start)) != std::string::npos) {
            // Edge case check: multiple continuous slashes like // ko handle karne ke liye
            if (end > start) {
                segments.push_back(path.substr(start, end - start));
            }
            start = end + 1;
        }
        if (start < path.length()) segments.push_back(path.substr(start));
        return segments;
    }

public:
    TrieRouter() : root(std::make_shared<TrieNode>()) {}

    void addRoute(std::string path, std::string backend) {
        auto curr = root;
        auto segments = splitPath(path);
        for (const auto& segment : segments) {
            if (curr->children.find(segment) == curr->children.end()) {
                curr->children[segment] = std::make_shared<TrieNode>();
            }
            curr = curr->children[segment];
        }
        curr->isEndOfPath = true;
        curr->backendUrl = backend;
    }

    std::string getBackend(std::string path) {
        auto curr = root;
        auto segments = splitPath(path);
        for (const auto& segment : segments) {
            if (curr->children.find(segment) == curr->children.end()) return "NOT_FOUND";
            curr = curr->children[segment];
        }
        return curr->isEndOfPath ? curr->backendUrl : "NOT_FOUND";
    }
};

#endif
