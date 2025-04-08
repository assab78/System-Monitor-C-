#ifndef RESOURCE_MONITOR_H
#define RESOURCE_MONITOR_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include "history_data.h"
#include "settings.h"
#include "notification_manager.h"

struct CPUStats {
    unsigned long long user;
    unsigned long long nice;
    unsigned long long system;
    unsigned long long idle;
    unsigned long long iowait;
    unsigned long long irq;
    unsigned long long softirq;
    unsigned long long steal;
    unsigned long long guest;
    unsigned long long guest_nice;
    
    CPUStats operator-(const CPUStats& other) const;
    unsigned long long total() const;
};

struct MemoryInfo {
    unsigned long long total;
    unsigned long long free;
    unsigned long long available;
    unsigned long long buffers;
    unsigned long long cached;
    unsigned long long used;
    double percent;
};

struct DiskInfo {
    std::string device;
    std::string mountpoint;
    unsigned long long total;
    unsigned long long used;
    unsigned long long available;
    double percent;
};

class ResourceMonitor {
public:
    ResourceMonitor(Settings* settings);
    ~ResourceMonitor();
    
    bool initialize();
    
    // Update all resources
    void update();
    
    // Get current CPU usage percentage
    double getCPUUsage() const;
    
    // Get current memory usage
    const MemoryInfo& getMemoryInfo() const;
    
    // Get current disk usage
    const std::vector<DiskInfo>& getDiskInfo() const;
    
    // Get history data
    HistoryData* getCPUHistory() const;
    HistoryData* getMemoryHistory() const;
    HistoryData* getDiskHistory(const std::string& mountpoint) const;
    
    // Check if any resource exceeds threshold
    bool checkThresholds(std::string& message, ResourceType& resourceType);
    
private:
    Settings* m_settings;
    
    CPUStats m_prevCPUStats;
    double m_cpuUsage;
    MemoryInfo m_memInfo;
    std::vector<DiskInfo> m_diskInfo;
    
    std::unique_ptr<HistoryData> m_cpuHistory;
    std::unique_ptr<HistoryData> m_memHistory;
    std::map<std::string, std::unique_ptr<HistoryData>> m_diskHistory;
    
    std::chrono::time_point<std::chrono::steady_clock> m_lastUpdate;
    
    bool readCPUStats(CPUStats& stats);
    bool readMemoryInfo();
    bool readDiskInfo();
};

#endif // RESOURCE_MONITOR_H
