#include "dreamnet_index.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

DreamnetIndex::DreamnetIndex(double carbon_limit) 
    : carbon_index_limit(carbon_limit) {}

void DreamnetIndex::recordSession(const DreamSession& session) {
    sessions.push_back(session);
    
    // Keep only last 1000 sessions for memory management
    if (sessions.size() > 1000) {
        sessions.erase(sessions.begin(), sessions.begin() + (sessions.size() - 1000));
    }
}

double DreamnetIndex::calculateCarbonIndex(const DreamSession& session) const {
    auto duration = session.end_time - session.start_time;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration).count();
    if (hours == 0) hours = 1;  // Avoid division by zero
    
    double carbon_kg = calculateSessionCarbon(session);
    return carbon_kg / hours;
}

bool DreamnetIndex::checkCarbonCompliance(const DreamSession& session) const {
    return calculateCarbonIndex(session) <= carbon_index_limit;
}

double DreamnetIndex::getTotalCarbonSaved() const {
    double conventional_carbon = 0.0;
    double dreamnet_carbon = 0.0;
    
    for (const auto& session : sessions) {
        // Conventional compute would run at full power during wake hours
        // Assume conventional is 5x more carbon intensive
        auto duration = session.end_time - session.start_time;
        auto hours = std::chrono::duration_cast<std::chrono::hours>(duration).count();
        
        double conventional_rate = 0.10;  // 0.10 kg CO2e/hour conventional
        double dreamnet_rate = calculateCarbonIndex(session);
        
        conventional_carbon += conventional_rate * hours;
        dreamnet_carbon += dreamnet_rate * hours;
    }
    
    return conventional_carbon - dreamnet_carbon;
}

DreamnetIndex::ComplianceStats DreamnetIndex::getComplianceStats() const {
    ComplianceStats stats{};
    stats.total_sessions = sessions.size();
    
    double total_carbon = 0.0;
    std::vector<double> carbon_indices;
    
    for (const auto& session : sessions) {
        double carbon_index = calculateCarbonIndex(session);
        carbon_indices.push_back(carbon_index);
        total_carbon += carbon_index * 
            std::chrono::duration_cast<std::chrono::hours>(
                session.end_time - session.start_time).count();
        
        if (carbon_index <= carbon_index_limit) {
            stats.compliant_sessions++;
        }
    }
    
    if (!carbon_indices.empty()) {
        stats.avg_carbon_index = std::accumulate(
            carbon_indices.begin(), carbon_indices.end(), 0.0) / carbon_indices.size();
    }
    
    stats.total_carbon_kg = total_carbon;
    return stats;
}

std::vector<std::chrono::system_clock::time_point> 
DreamnetIndex::recommendComputeWindows(int hours_ahead) const {
    std::vector<std::chrono::system_clock::time_point> windows;
    
    // Simple heuristic: recommend windows during typical sleep hours
    // (10 PM to 6 AM) when grid intensity is lower
    auto now = std::chrono::system_clock::now();
    
    for (int i = 0; i < hours_ahead; i++) {
        auto future_time = now + std::chrono::hours(i);
        auto time_t = std::chrono::system_clock::to_time_t(future_time);
        std::tm tm = *std::localtime(&time_t);
        
        // Recommend sleep hours (10 PM to 6 AM)
        if (tm.tm_hour >= 22 || tm.tm_hour < 6) {
            windows.push_back(future_time);
        }
    }
    
    return windows;
}

void DreamnetIndex::setCarbonLimit(double limit_kg_co2e_per_hour) {
    carbon_index_limit = limit_kg_co2e_per_hour;
}

double DreamnetIndex::calculateSessionCarbon(const DreamSession& session) const {
    auto duration = session.end_time - session.start_time;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration).count();
    
    // Carbon = Power (kW) * Time (h) * Carbon Intensity (g CO2/kWh) / 1000
    double power_kw = session.compute_power_watts / 1000.0;
    double carbon_intensity = getGridIntensityAtTime(session.start_time);
    
    return power_kw * hours * carbon_intensity / 1000.0;  // Convert to kg
}

double DreamnetIndex::getGridIntensityAtTime(
    const std::chrono::system_clock::time_point& time) const {
    // Default Phoenix lab target: 80 g CO2/kWh
    // Could be enhanced with real-time grid data
    return 80.0;
}
