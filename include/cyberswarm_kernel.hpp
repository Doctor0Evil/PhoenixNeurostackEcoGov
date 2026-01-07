
## 3. include/cyberswarm_kernel.hpp

```cpp
// File: include/cyberswarm_kernel.hpp
// Destination: PhoenixNeurostackEcoGov/include/cyberswarm_kernel.hpp
#pragma once
#include <vector>
#include <string>
#include "neuro_node.hpp"

struct SafetyConstraint {
    std::string axis;  // intensity, duty, load, power, neuromod_amplitude, cognitive_load, legal_complexity
    double min_value;
    double max_value;
    double current_value;
    bool is_violated;
};

class CyberswarmKernel {
private:
    std::vector<SafetyConstraint> constraints;
    const int AXIS_COUNT = 7;
    
public:
    CyberswarmKernel();
    
    // Load constraints from neuro nodes
    void loadConstraints(const std::vector<NeuroNode>& nodes);
    
    // Check if a proposed action violates constraints
    bool checkViability(const std::vector<double>& proposed_action) const;
    
    // Get constraint violation report
    std::string getViolationReport() const;
    
    // Update constraint values (for simulation/telemetry)
    void updateConstraint(const std::string& axis, double value);
    
    // Calculate safety margin (0-1, higher is safer)
    double calculateSafetyMargin() const;
    
    // Get all constraints
    const std::vector<SafetyConstraint>& getConstraints() const { return constraints; }
    
private:
    void initializeDefaultConstraints();
    SafetyConstraint* findConstraint(const std::string& axis);
};
