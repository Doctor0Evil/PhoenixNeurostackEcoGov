#include "cyberswarm_kernel.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cmath>

CyberswarmKernel::CyberswarmKernel() {
    initializeDefaultConstraints();
}

void CyberswarmKernel::initializeDefaultConstraints() {
    constraints = {
        {"intensity", 0.0, 1.0, 0.0, false},
        {"duty", 0.0, 0.8, 0.0, false},
        {"load", 0.0, 100.0, 0.0, false},
        {"power", 0.0, 750.0, 0.0, false},  // 750 mW limit
        {"neuromod_amplitude", 0.0, 1.0, 0.0, false},
        {"cognitive_load", 0.0, 0.7, 0.0, false},  // Max 0.7 normalized
        {"legal_complexity", 0.0, 1.0, 0.0, false}
    };
}

void CyberswarmKernel::loadConstraints(const std::vector<NeuroNode>& nodes) {
    for (const auto& node : nodes) {
        if (node.layer == "GovSafety") {
            if (node.parameter == "SafetyKernelDim") {
                // Axes count validation
                if (node.value != AXIS_COUNT) {
                    // Log warning or adjust
                }
            } else if (node.parameter == "MaxCognitiveLoadIndex") {
                updateConstraint("cognitive_load", node.value);
            }
        } else if (node.layer == "BCIIngress") {
            if (node.parameter == "GatewayPowerDraw") {
                updateConstraint("power", node.value);
            }
        }
    }
}

bool CyberswarmKernel::checkViability(const std::vector<double>& proposed_action) const {
    if (proposed_action.size() != AXIS_COUNT) {
        return false;
    }
    
    for (size_t i = 0; i < constraints.size(); ++i) {
        if (proposed_action[i] < constraints[i].min_value || 
            proposed_action[i] > constraints[i].max_value) {
            return false;
        }
    }
    
    return true;
}

std::string CyberswarmKernel::getViolationReport() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << "Cyberswarm Safety Kernel Violation Report\n";
    ss << "=========================================\n";
    
    bool any_violation = false;
    for (const auto& constraint : constraints) {
        if (constraint.is_violated) {
            any_violation = true;
            ss << "Axis: " << constraint.axis << "\n";
            ss << "  Current: " << constraint.current_value << "\n";
            ss << "  Allowed: [" << constraint.min_value << ", " << constraint.max_value << "]\n";
            ss << "  Violation: ";
            if (constraint.current_value < constraint.min_value) {
                ss << "Below minimum by " << (constraint.min_value - constraint.current_value) << "\n";
            } else {
                ss << "Above maximum by " << (constraint.current_value - constraint.max_value) << "\n";
            }
        }
    }
    
    if (!any_violation) {
        ss << "All constraints satisfied.\n";
    }
    
    ss << "Safety Margin: " << calculateSafetyMargin() * 100 << "%\n";
    return ss.str();
}

void CyberswarmKernel::updateConstraint(const std::string& axis, double value) {
    auto constraint = findConstraint(axis);
    if (constraint) {
        constraint->current_value = value;
        constraint->is_violated = (value < constraint->min_value || value > constraint->max_value);
    }
}

double CyberswarmKernel::calculateSafetyMargin() const {
    double total_margin = 0.0;
    int valid_constraints = 0;
    
    for (const auto& constraint : constraints) {
        double range = constraint.max_value - constraint.min_value;
        if (range > 0) {
            // Calculate normalized distance from limits (0-1, higher is safer)
            double distance_from_min = (constraint.current_value - constraint.min_value) / range;
            double distance_from_max = (constraint.max_value - constraint.current_value) / range;
            double margin = std::min(distance_from_min, distance_from_max);
            
            // Clamp to [0,1] and accumulate
            margin = std::max(0.0, std::min(1.0, margin));
            total_margin += margin;
            valid_constraints++;
        }
    }
    
    return valid_constraints > 0 ? total_margin / valid_constraints : 0.0;
}

SafetyConstraint* CyberswarmKernel::findConstraint(const std::string& axis) {
    for (auto& constraint : constraints) {
        if (constraint.axis == axis) {
            return &constraint;
        }
    }
    return nullptr;
}
