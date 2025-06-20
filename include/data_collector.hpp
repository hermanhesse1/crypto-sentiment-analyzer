#pragma once
#include <vector>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <memory>
#include "config_manager.hpp"

struct MarketData {
    std::string source;     // "reddit", "twitter", "news"
    std::string symbol;     // Crypto symbol
    std::string text;       // Content
    std::chrono::system_clock::time_point timestamp;
};

class DataCollector {
private:
    std::queue<MarketData> data_queue_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};
    std::vector<std::thread> collector_threads_;
    
    void collect_reddit_data();
    void collect_twitter_data();
    void collect_news_data();
    void add_data(const MarketData& data);
    
public:
    explicit DataCollector(const ConfigManager& config);
    ~DataCollector();
    
    void start();
    void stop();
    std::vector<MarketData> get_latest_data();
};