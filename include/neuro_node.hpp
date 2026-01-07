#pragma once
#include <string>

struct NeuroNode {
    std::string nodeId;
    std::string layer;
    std::string region;
    double latitude;
    double longitude;
    std::string parameter;
    std::string unit;
    double value;
    std::string window;
    double ecoImpactScore;
    std::string notes;
};
