# 🚀 Lighthouse API Gateway v5.0

A high-performance custom-built API Gateway implemented in C++ with a live Web-based Simulator.

## 🌟 Key Features

⚡ In-Memory Response Caching:** Reduces repeated request latency to **0ms**.
🛡️ IP Whitelisting (Firewall):** Blocks unauthorized IP addresses before they reach the backend.
🚦 Dynamic Rate Limiting:** Prevents DDoS attacks (e.g., 3 requests per 10s).
🌳 Trie-Based Routing:** Efficient $O(L)$ route lookups using Prefix Trees.
🔗 Middleware Chain:** Modular Auth, Logging, and Security layers.

---

🛠️ Tech Stack

Core: C++ (OOPs, STL, Smart Pointers)
Data Structures: Trie, Hash Maps, Sets
Frontend: HTML5, CSS3, JavaScript (ES6)

---

## 📂 Project Structure

- include/ : Header files (`.hpp`)
- src/ : Source logic (`main.cpp`)
- build/ : Compiled executables
- config.txt : Route configurations
- index.html : Live Gateway Simulator

---

 🚀 How to Run (Terminal)

1. Compile: `g++ src/main.cpp -I include -o build/gateway`

2. Run: `./build/gateway`

---

## 🌐 Live Simulator
Test the Gateway logic directly in your browser:  
👉 **[Yahan Apna GitHub Link Daalein]**
