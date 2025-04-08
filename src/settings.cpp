#include "settings.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

Settings::Settings()
    : m_cpuThreshold(85.0),          // Default: 85% (было 80%)
      m_memoryThreshold(85.0),       // Default: 85% (было 80%)
      m_diskThreshold(90.0),         // Default: 90%
      m_updateInterval(1000),        // Default: 1 second
      m_notificationCooldown(300)    // Default: 300 seconds (5 минут, было 60 секунд)
{
    // Set config path to ~/.config/system-monitor/settings.conf
    const char* homeDir = getenv("HOME");
    if (homeDir) {
        m_configPath = std::string(homeDir) + "/.config/system-monitor/settings.conf";
        
        // Create directory if it doesn't exist
        std::filesystem::path configDir = std::string(homeDir) + "/.config/system-monitor";
        if (!std::filesystem::exists(configDir)) {
            try {
                std::filesystem::create_directories(configDir);
            } catch (const std::exception& e) {
                std::cerr << "Failed to create config directory: " << e.what() << std::endl;
            }
        }
    } else {
        m_configPath = "settings.conf";
    }
}

Settings::~Settings() {
    // Default destructor
}

bool Settings::save() {
    try {
        std::ofstream file(m_configPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open config file for writing: " << m_configPath << std::endl;
            return false;
        }
        
        file << "cpu_threshold=" << m_cpuThreshold << std::endl;
        file << "memory_threshold=" << m_memoryThreshold << std::endl;
        file << "disk_threshold=" << m_diskThreshold << std::endl;
        file << "update_interval=" << m_updateInterval << std::endl;
        file << "notification_cooldown=" << m_notificationCooldown << std::endl;
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving settings: " << e.what() << std::endl;
        return false;
    }
}

bool Settings::load() {
    try {
        std::ifstream file(m_configPath);
        if (!file.is_open()) {
            std::cerr << "Config file not found, using defaults: " << m_configPath << std::endl;
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            std::string value;
            
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                if (key == "cpu_threshold") {
                    m_cpuThreshold = std::stod(value);
                } else if (key == "memory_threshold") {
                    m_memoryThreshold = std::stod(value);
                } else if (key == "disk_threshold") {
                    m_diskThreshold = std::stod(value);
                } else if (key == "update_interval") {
                    m_updateInterval = std::stoi(value);
                } else if (key == "notification_cooldown") {
                    m_notificationCooldown = std::stoi(value);
                }
            }
        }
        
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading settings: " << e.what() << std::endl;
        return false;
    }
}

double Settings::getCPUThreshold() const {
    return m_cpuThreshold;
}

double Settings::getMemoryThreshold() const {
    return m_memoryThreshold;
}

double Settings::getDiskThreshold() const {
    return m_diskThreshold;
}

int Settings::getUpdateInterval() const {
    return m_updateInterval;
}

int Settings::getNotificationCooldown() const {
    return m_notificationCooldown;
}

void Settings::setCPUThreshold(double threshold) {
    m_cpuThreshold = threshold;
    notifyChange();
}

void Settings::setMemoryThreshold(double threshold) {
    m_memoryThreshold = threshold;
    notifyChange();
}

void Settings::setDiskThreshold(double threshold) {
    m_diskThreshold = threshold;
    notifyChange();
}

void Settings::setUpdateInterval(int interval) {
    m_updateInterval = interval;
    notifyChange();
}

void Settings::setNotificationCooldown(int cooldown) {
    m_notificationCooldown = cooldown;
    notifyChange();
}

void Settings::registerChangeCallback(std::function<void()> callback) {
    m_changeCallbacks.push_back(callback);
}

void Settings::notifyChange() {
    for (const auto& callback : m_changeCallbacks) {
        callback();
    }
}
