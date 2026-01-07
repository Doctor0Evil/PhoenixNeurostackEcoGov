#include <iostream>
#include <iomanip>
#include "eco_metrics.hpp"

int main() {
    const std::string shardPath = "data/PhoenixNeurostackEcoGov2026v1.csv";
    try {
        auto nodes = loadNeurostackEcoShard(shardPath);
        if (nodes.empty()) {
            std::cerr << "No nodes loaded from " << shardPath << "\n";
            return 1;
        }
        EcoSummary summary = computeEcoSummary(nodes);

        std::cout << "Phoenix Neurostack Eco-Governance Summary 2026\n";
        std::cout << "Nodes loaded: " << nodes.size() << "\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Average Eco-Impact Score (all nodes): " 
                  << summary.avgEcoImpactScore << "\n";
        std::cout << "Water-linked Eco-Impact (PFBS, E. coli): " 
                  << summary.ecoImpactScoreWater << "\n";
        std::cout << "Energy-linked Eco-Impact (lab intensity): " 
                  << summary.ecoImpactScoreEnergy << "\n";
        std::cout << "BCI & Governance Eco-Impact: " 
                  << summary.ecoImpactScoreBCI << "\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
