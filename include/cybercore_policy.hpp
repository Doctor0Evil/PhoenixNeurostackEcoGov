#pragma once
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include "neuro_node.hpp"

enum class StakeholderType {
    CLINICIAN,
    ENGINEER,
    ETHICIST,
    AUGMENTED_USER,
    COMMUNITY_REP,
    REGULATOR
};

struct StakeholderVote {
    StakeholderType type;
    std::string identifier;  // DID or other ID
    bool approval;
    std::chrono::system_clock::time_point timestamp;
    std::string rationale;
};

struct GovernanceDecision {
    std::string decision_id;
    std::string description;
    std::vector<StakeholderVote> votes;
    double consensus_threshold = 0.7;  // 70% default
    bool approved;
    std::chrono::system_clock::time_point decision_time;
    
    // NeuroRights tags
    std::vector<std::string> neurorights_tags;
};

class CybercorePolicy {
private:
    std::vector<GovernanceDecision> decisions;
    std::map<StakeholderType, int> stakeholder_weights;
    double current_consensus_threshold = 0.7;
    
public:
    CybercorePolicy();
    
    // Propose a new decision
    std::string proposeDecision(const std::string& description, 
                               const std::vector<std::string>& neurorights_tags = {});
    
    // Cast a vote on a decision
    bool castVote(const std::string& decision_id, 
                 StakeholderType type,
                 const std::string& identifier,
                 bool approval,
                 const std::string& rationale = "");
    
    // Finalize decision (calculate approval)
    bool finalizeDecision(const std::string& decision_id);
    
    // Check if a decision would pass with current votes
    bool wouldPass(const std::string& decision_id) const;
    
    // Get decision by ID
    const GovernanceDecision* getDecision(const std::string& decision_id) const;
    
    // Get all decisions with given neurorights tag
    std::vector<GovernanceDecision> getDecisionsByTag(const std::string& tag) const;
    
    // Check neurorights policy coverage (0-100%)
    double calculatePolicyCoverage() const;
    
    // Set stakeholder weights
    void setStakeholderWeight(StakeholderType type, int weight);
    
    // Set consensus threshold
    void setConsensusThreshold(double threshold);
    
    // Generate compliance report
    struct ComplianceReport {
        int total_decisions;
        int approved_decisions;
        double approval_rate;
        double policy_coverage;
        std::map<std::string, int> tag_distribution;
    };
    
    ComplianceReport generateComplianceReport() const;
    
private:
    GovernanceDecision* findDecision(const std::string& decision_id);
    const GovernanceDecision* findDecision(const std::string& decision_id) const;
    double calculateApprovalRate(const GovernanceDecision& decision) const;
    bool checkNeurorightsTags(const std::vector<std::string>& tags) const;
};
