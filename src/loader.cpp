#include "eco_metrics.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::vector<NeuroNode> loadNeurostackEcoShard(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open CSV: " + csvPath);
    }
    std::vector<NeuroNode> nodes;
    std::string line;
    // skip header
    if (!std::getline(file, line)) return nodes;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string field;
        std::vector<std::string> fields;
        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }
        if (fields.size() < 12) continue;

        NeuroNode n;
        n.nodeId      = fields[0];
        n.layer       = fields[1];
        n.region      = fields[2];
        n.latitude    = std::stod(fields[3]);
        n.longitude   = std::stod(fields[4]);
        n.parameter   = fields[5];
        n.unit        = fields[6];
        n.value       = std::stod(fields[7]);
        n.window      = fields[8];
        n.ecoImpactScore = std::stod(fields[9]);
        n.notes       = fields[10];
        nodes.push_back(n);
    }
    return nodes;
}
