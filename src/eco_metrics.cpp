#include "eco_metrics.hpp"

EcoSummary computeEcoSummary(const std::vector<NeuroNode>& nodes) {
    EcoSummary s{0.0, 0.0, 0.0, 0.0};
    double sumAll = 0.0; int countAll = 0;
    double sumWater = 0.0; int countWater = 0;
    double sumEnergy = 0.0; int countEnergy = 0;
    double sumBCI = 0.0; int countBCI = 0;

    for (const auto& n : nodes) {
        sumAll += n.ecoImpactScore; ++countAll;

        if (n.layer == "EcoLink" && 
           (n.parameter.find("PFBS") != std::string::npos ||
            n.parameter.find("Ecoli") != std::string::npos)) {
            sumWater += n.ecoImpactScore; ++countWater;
        }
        if (n.layer == "EcoLink" && 
            n.parameter.find("ElectricityIntensity") != std::string::npos) {
            sumEnergy += n.ecoImpactScore; ++countEnergy;
        }
        if (n.layer == "BCIIngress" || n.layer == "GovSafety" || n.layer == "GovOS") {
            sumBCI += n.ecoImpactScore; ++countBCI;
        }
    }

    s.avgEcoImpactScore   = countAll   ? sumAll   / countAll   : 0.0;
    s.ecoImpactScoreWater = countWater ? sumWater / countWater : 0.0;
    s.ecoImpactScoreEnergy= countEnergy? sumEnergy/ countEnergy: 0.0;
    s.ecoImpactScoreBCI   = countBCI   ? sumBCI   / countBCI   : 0.0;
    return s;
}
