#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>
#include <functional>

// Notification threshold settings
class Settings {
public:
    Settings();
    ~Settings();
    
    // Save settings to file
    bool save();
    
    // Load settings from file
    bool load();
    
    // Getters
    double getCPUThreshold() const;
    double getMemoryThreshold() const;
    double getDiskThreshold() const;
    int getUpdateInterval() const;
    int getNotificationCooldown() const;
    
    // Setters
    void setCPUThreshold(double threshold);
    void setMemoryThreshold(double threshold);
    void setDiskThreshold(double threshold);
    void setUpdateInterval(int interval);
    void setNotificationCooldown(int cooldown);
    
    // Register callback for settings changes
    void registerChangeCallback(std::function<void()> callback);

private:
    double m_cpuThreshold;      // Percentage threshold for CPU usage
    double m_memoryThreshold;   // Percentage threshold for memory usage
    double m_diskThreshold;     // Percentage threshold for disk usage
    int m_updateInterval;       // Update interval in milliseconds
    int m_notificationCooldown; // Cooldown between notifications in seconds
    
    std::string m_configPath;
    std::vector<std::function<void()>> m_changeCallbacks;
    
    // Notify all registered callbacks
    void notifyChange();
};

#endif // SETTINGS_H
