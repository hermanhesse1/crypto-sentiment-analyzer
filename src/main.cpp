#include <iostream>
#include <thread>
#include <chrono>
#include <signal.h>
#include "sentiment_analyzer.hpp"
#include "data_collector.hpp"
#include "signal_generator.hpp"
#include "config_manager.hpp"

std::atomic<bool> g_running(true);

void signal_handler(int signal) {
    std::cout << "\nShutting down gracefully..." << std::endl;
    g_running = false;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signal_handler);
    
    ConfigManager config;
    if (!config.load("config.json")) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }
    
    DataCollector collector(config);
    SentimentAnalyzer analyzer(config);
    SignalGenerator signal_gen(config);
    
    collector.start();
    
    while (g_running) {
        auto data = collector.get_latest_data();
        
        if (!data.empty()) {
            auto sentiments = analyzer.analyze_batch(data);
            auto signals = signal_gen.generate_signals(sentiments);
            
            for (const auto& signal : signals) {
                std::cout << "[" << signal.timestamp << "] " 
                         << signal.symbol << ": " 
                         << "Sentiment=" << signal.sentiment_score 
                         << ", Signal=" << signal.trading_signal 
                         << ", Confidence=" << signal.confidence << std::endl;
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    collector.stop();
    return 0;
}
