#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <deque>
#include <chrono>
#include "config_manager.hpp"
#include "data_collector.hpp"

struct SentimentData {
    std::string symbol;
    double score;           // -1.0 to 1.0
    double volume;          // Message volume
    double confidence;      // 0.0 to 1.0
    std::chrono::system_clock::time_point timestamp;
};

class SentimentAnalyzer {
private:
    struct SentimentCache {
        std::deque<SentimentData> history;
        std::mutex mutex;
        std::chrono::minutes window_size{15};
    };
    
    std::unordered_map<std::string, std::unique_ptr<SentimentCache>> cache_;
    std::atomic<size_t> processed_count_{0};
    
    double calculate_sentiment(const std::string& text);
    void update_cache(const std::string& symbol, const SentimentData& data);
    
public:
    explicit SentimentAnalyzer(const ConfigManager& config);
    
    std::vector<SentimentData> analyze_batch(const std::vector<MarketData>& data);
    double get_weighted_sentiment(const std::string& symbol);
    std::unordered_map<std::string, double> get_all_sentiments();
};