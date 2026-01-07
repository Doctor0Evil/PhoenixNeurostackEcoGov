#pragma once
#include <vector>
#include "neuro_node.hpp"

struct EcoSummary {
    double avgEcoImpactScore;
    double ecoImpactScoreWater;
    double ecoImpactScoreEnergy;
    double ecoImpactScoreBCI;
};

std::vector<NeuroNode> loadNeurostackEcoShard(const std::string& csvPath);
EcoSummary computeEcoSummary(const std::vector<NeuroNode>& nodes);
