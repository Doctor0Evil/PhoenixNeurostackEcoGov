#pragma once
#include <string>
#include <chrono>
#include <vector>

struct DreamSession {
    std::chrono::system_clock::time_point start_time;
    std::chrono::system_clock::time_point end_time;
    
    // Sleep metrics
    double sleep_efficiency;  // 0-1
    std::string sleep_stage;  // N1, N2, N3, REM
    
    // Compute metrics
    double compute_power_watts;
    double carbon_intensity_gco2_per_kwh;  // g CO2/kWh
    
    // Content metrics
    bool content_persisted;  // Should be false for privacy
    int dream_state_features;  // Structural features only
};

class DreamnetIndex {
private:
    double carbon_index_limit = 0.02;  // kg CO2e/hour default
    std::vector<DreamSession> sessions;
    
public:
    DreamnetIndex(double carbon_limit = 0.02);
    
    // Record a new dream session
    void recordSession(const DreamSession& session);
    
    // Calculate carbon index for a session
    double calculateCarbonIndex(const DreamSession& session) const;
    
    // Check if session violates carbon limits
    bool checkCarbonCompliance(const DreamSession& session) const;
    
    // Get total carbon saved by dream-gating
    double getTotalCarbonSaved() const;
    
    // Get compliance statistics
    struct ComplianceStats {
        int total_sessions;
        int compliant_sessions;
        double avg_carbon_index;
        double total_carbon_kg;
    };
    
    ComplianceStats getComplianceStats() const;
    
    // Recommend optimal compute windows based on sleep patterns
    std::vector<std::chrono::system_clock::time_point> 
    recommendComputeWindows(int hours_ahead = 24) const;
    
    // Set carbon index limit (from policy)
    void setCarbonLimit(double limit_kg_co2e_per_hour);
    
private:
    double calculateSessionCarbon(const DreamSession& session) const;
    double getGridIntensityAtTime(const std::chrono::system_clock::time_point& time) const;
};
