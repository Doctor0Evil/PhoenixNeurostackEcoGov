#include "cybercore_policy.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <random>

CybercorePolicy::CybercorePolicy() {
    // Default equal weights
    stakeholder_weights = {
        {StakeholderType::CLINICIAN, 1},
        {StakeholderType::ENGINEER, 1},
        {StakeholderType::ETHICIST, 1},
        {StakeholderType::AUGMENTED_USER, 1},
        {StakeholderType::COMMUNITY_REP, 1},
        {StakeholderType::REGULATOR, 1}
    };
}

std::string CybercorePolicy::proposeDecision(
    const std::string& description, 
    const std::vector<std::string>& neurorights_tags) {
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    
    GovernanceDecision decision;
    decision.decision_id = "DEC-" + std::to_string(dis(gen));
    decision.description = description;
    decision.neurorights_tags = neurorights_tags;
    decision.approved = false;
    decision.decision_time = std::chrono::system_clock::now();
    
    // Validate neurorights tags
    if (!checkNeurorightsTags(neurorights_tags)) {
        return "";  // Invalid tags
    }
    
    decisions.push_back(decision);
    return decision.decision_id;
}

bool CybercorePolicy::castVote(
    const std::string& decision_id,
    StakeholderType type,
    const std::string& identifier,
    bool approval,
    const std::string& rationale) {
    
    auto decision = findDecision(decision_id);
    if (!decision) {
        return false;
    }
    
    // Check if stakeholder already voted
    for (const auto& vote : decision->votes) {
        if (vote.type == type && vote.identifier == identifier) {
            return false;  // Already voted
        }
    }
    
    StakeholderVote vote;
    vote.type = type;
    vote.identifier = identifier;
    vote.approval = approval;
    vote.rationale = rationale;
    vote.timestamp = std::chrono::system_clock::now();
    
    decision->votes.push_back(vote);
    return true;
}

bool CybercorePolicy::finalizeDecision(const std::string& decision_id) {
    auto decision = findDecision(decision_id);
    if (!decision) {
        return false;
    }
    
    decision->approved = wouldPass(decision_id);
    return decision->approved;
}

bool CybercorePolicy::wouldPass(const std::string& decision_id) const {
    const auto* decision = findDecision(decision_id);
    if (!decision) {
        return false;
    }
    
    return calculateApprovalRate(*decision) >= decision->consensus_threshold;
}

const GovernanceDecision* CybercorePolicy::getDecision(
    const std::string& decision_id) const {
    return findDecision(decision_id);
}

std::vector<GovernanceDecision> CybercorePolicy::getDecisionsByTag(
    const std::string& tag) const {
    
    std::vector<GovernanceDecision> result;
    for (const auto& decision : decisions) {
        if (std::find(decision.neurorights_tags.begin(), 
                     decision.neurorights_tags.end(), tag) != decision.neurorights_tags.end()) {
            result.push_back(decision);
        }
    }
    return result;
}

double CybercorePolicy::calculatePolicyCoverage() const {
    if (decisions.empty()) {
        return 100.0;  // No decisions means no violations
    }
    
    int tagged_decisions = 0;
    for (const auto& decision : decisions) {
        if (!decision.neurorights_tags.empty()) {
            tagged_decisions++;
        }
    }
    
    return (static_cast<double>(tagged_decisions) / decisions.size()) * 100.0;
}

void CybercorePolicy::setStakeholderWeight(StakeholderType type, int weight) {
    stakeholder_weights[type] = weight;
}

void CybercorePolicy::setConsensusThreshold(double threshold) {
    current_consensus_threshold = threshold;
}

CybercorePolicy::ComplianceReport CybercorePolicy::generateComplianceReport() const {
    ComplianceReport report{};
    report.total_decisions = decisions.size();
    
    std::map<std::string, int> tag_counts;
    
    for (const auto& decision : decisions) {
        if (decision.approved) {
            report.approved_decisions++;
        }
        
        for (const auto& tag : decision.neurorights_tags) {
            tag_counts[tag]++;
        }
    }
    
    if (report.total_decisions > 0) {
        report.approval_rate = static_cast<double>(report.approved_decisions) / 
                              report.total_decisions;
    }
    
    report.policy_coverage = calculatePolicyCoverage();
    report.tag_distribution = tag_counts;
    
    return report;
}

GovernanceDecision* CybercorePolicy::findDecision(const std::string& decision_id) {
    for (auto& decision : decisions) {
        if (decision.decision_id == decision_id) {
            return &decision;
        }
    }
    return nullptr;
}

const GovernanceDecision* CybercorePolicy::findDecision(
    const std::string& decision_id) const {
    
    for (const auto& decision : decisions) {
        if (decision.decision_id == decision_id) {
            return &decision;
        }
    }
    return nullptr;
}

double CybercorePolicy::calculateApprovalRate(
    const GovernanceDecision& decision) const {
    
    if (decision.votes.empty()) {
        return 0.0;
    }
    
    double total_weight = 0.0;
    double approval_weight = 0.0;
    
    for (const auto& vote : decision.votes) {
        double weight = stakeholder_weights.at(vote.type);
        total_weight += weight;
        if (vote.approval) {
            approval_weight += weight;
        }
    }
    
    return total_weight > 0 ? approval_weight / total_weight : 0.0;
}

bool CybercorePolicy::checkNeurorightsTags(
    const std::vector<std::string>& tags) const {
    
    // Valid neurorights tags from Phoenix Neurostack specification
    static const std::vector<std::string> valid_tags = {
        "neurorights.privacy",
        "neurorights.agency",
        "neurorights.identity",
        "neurorights.equality",
        "neurorights.protection",
        "neurorights.freedom"
    };
    
    for (const auto& tag : tags) {
        if (std::find(valid_tags.begin(), valid_tags.end(), tag) == valid_tags.end()) {
            // Check if it's a valid prefix
            bool valid_prefix = false;
            for (const auto& valid_tag : valid_tags) {
                if (tag.find(valid_tag) == 0) {  // Starts with valid tag
                    valid_prefix = true;
                    break;
                }
            }
            if (!valid_prefix) {
                return false;
            }
        }
    }
    
    return true;
}
