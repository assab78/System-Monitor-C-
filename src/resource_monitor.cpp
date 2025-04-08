#include "resource_monitor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <sys/statvfs.h>
#include <mntent.h>

CPUStats CPUStats::operator-(const CPUStats& other) const {
    CPUStats result;
    result.user = user - other.user;
    result.nice = nice - other.nice;
    result.system = system - other.system;
    result.idle = idle - other.idle;
    result.iowait = iowait - other.iowait;
    result.irq = irq - other.irq;
    result.softirq = softirq - other.softirq;
    result.steal = steal - other.steal;
    result.guest = guest - other.guest;
    result.guest_nice = guest_nice - other.guest_nice;
    return result;
}

unsigned long long CPUStats::total() const {
    return user + nice + system + idle + iowait + irq + softirq + steal;
}

ResourceMonitor::ResourceMonitor(Settings* settings)
    : m_settings(settings),
      m_cpuUsage(0.0),
      m_lastUpdate(std::chrono::steady_clock::now())
{
    // Default constructor
}

ResourceMonitor::~ResourceMonitor() {
    // Clean up
}

bool ResourceMonitor::initialize() {
    // Initialize history data containers
    std::size_t historySize = 600; // Store 10 minutes of data with 1 second interval
    m_cpuHistory = std::make_unique<HistoryData>(historySize);
    m_memHistory = std::make_unique<HistoryData>(historySize);
    
    // Read initial CPU stats
    if (!readCPUStats(m_prevCPUStats)) {
        std::cerr << "Failed to read initial CPU stats!" << std::endl;
        return false;
    }
    
    // Read initial memory info
    if (!readMemoryInfo()) {
        std::cerr << "Failed to read memory info!" << std::endl;
        return false;
    }
    
    // Read initial disk info
    if (!readDiskInfo()) {
        std::cerr << "Failed to read disk info!" << std::endl;
        return false;
    }
    
    // История дисков уже инициализирована в readDiskInfo()
    
    return true;
}

void ResourceMonitor::update() {
    // Get current time
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        currentTime - m_lastUpdate).count();
    
    // Update only if enough time has passed (1 second)
    if (elapsed >= 1000) {
        m_lastUpdate = currentTime;
        
        // Update CPU usage
        CPUStats currentStats;
        if (readCPUStats(currentStats)) {
            CPUStats diff = currentStats - m_prevCPUStats;
            unsigned long long total = diff.total();
            unsigned long long idle = diff.idle + diff.iowait;
            
            if (total > 0) {
                m_cpuUsage = 100.0 * (total - idle) / total;
                m_cpuHistory->addSample(m_cpuUsage);
            }
            
            m_prevCPUStats = currentStats;
        }
        
        // Update memory info
        if (readMemoryInfo()) {
            m_memHistory->addSample(m_memInfo.percent);
        }
        
        // Update disk info
        if (readDiskInfo()) {
            for (const auto& disk : m_diskInfo) {
                m_diskHistory[disk.mountpoint]->addSample(disk.percent);
            }
        }
    }
}

double ResourceMonitor::getCPUUsage() const {
    return m_cpuUsage;
}

const MemoryInfo& ResourceMonitor::getMemoryInfo() const {
    return m_memInfo;
}

const std::vector<DiskInfo>& ResourceMonitor::getDiskInfo() const {
    return m_diskInfo;
}

HistoryData* ResourceMonitor::getCPUHistory() const {
    return m_cpuHistory.get();
}

HistoryData* ResourceMonitor::getMemoryHistory() const {
    return m_memHistory.get();
}

HistoryData* ResourceMonitor::getDiskHistory(const std::string& mountpoint) const {
    auto it = m_diskHistory.find(mountpoint);
    if (it != m_diskHistory.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool ResourceMonitor::checkThresholds(std::string& message, ResourceType& resourceType) {
    // Check CPU usage threshold
    if (m_cpuUsage >= m_settings->getCPUThreshold()) {
        message = "Высокая загрузка ЦП: " + std::to_string(static_cast<int>(m_cpuUsage)) + "%";
        resourceType = ResourceType::CPU;
        return true;
    }
    
    // Check memory usage threshold
    if (m_memInfo.percent >= m_settings->getMemoryThreshold()) {
        message = "Высокое использование памяти: " + std::to_string(static_cast<int>(m_memInfo.percent)) + "%";
        resourceType = ResourceType::Memory;
        return true;
    }
    
    // Check disk usage thresholds
    for (const auto& disk : m_diskInfo) {
        if (disk.percent >= m_settings->getDiskThreshold()) {
            message = "Высокое использование диска " + disk.mountpoint + ": " +
                      std::to_string(static_cast<int>(disk.percent)) + "%";
            resourceType = ResourceType::Disk;
            return true;
        }
    }
    
    resourceType = ResourceType::Other;
    return false;
}

bool ResourceMonitor::readCPUStats(CPUStats& stats) {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        std::cerr << "Failed to open /proc/stat" << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 3) == "cpu" && line[3] == ' ') {
            std::istringstream iss(line.substr(5)); // Skip "cpu" prefix
            iss >> stats.user >> stats.nice >> stats.system >> stats.idle
                >> stats.iowait >> stats.irq >> stats.softirq >> stats.steal
                >> stats.guest >> stats.guest_nice;
            return true;
        }
    }
    
    std::cerr << "CPU stats not found in /proc/stat" << std::endl;
    return false;
}

bool ResourceMonitor::readMemoryInfo() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) {
        std::cerr << "Failed to open /proc/meminfo" << std::endl;
        return false;
    }
    
    std::string line;
    unsigned long long memTotal = 0;
    unsigned long long memFree = 0;
    unsigned long long memAvailable = 0;
    unsigned long long buffers = 0;
    unsigned long long cached = 0;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        unsigned long long value;
        std::string unit;
        
        iss >> key >> value >> unit;
        
        if (key == "MemTotal:") {
            memTotal = value;
        } else if (key == "MemFree:") {
            memFree = value;
        } else if (key == "MemAvailable:") {
            memAvailable = value;
        } else if (key == "Buffers:") {
            buffers = value;
        } else if (key == "Cached:") {
            cached = value;
        }
    }
    
    if (memTotal == 0) {
        std::cerr << "Memory total is zero or not found" << std::endl;
        return false;
    }
    
    m_memInfo.total = memTotal;
    m_memInfo.free = memFree;
    m_memInfo.available = memAvailable;
    m_memInfo.buffers = buffers;
    m_memInfo.cached = cached;
    m_memInfo.used = memTotal - memFree - buffers - cached;
    
    // Calculate percentage
    m_memInfo.percent = 100.0 * (memTotal - memAvailable) / memTotal;
    
    return true;
}

bool ResourceMonitor::readDiskInfo() {
    FILE* mtab = setmntent("/etc/mtab", "r");
    if (!mtab) {
        std::cerr << "Failed to open /etc/mtab" << std::endl;
        return false;
    }
    
    m_diskInfo.clear();
    
    struct mntent* entry;
    while ((entry = getmntent(mtab)) != nullptr) {
        // Skip non-physical filesystems
        std::string fstype(entry->mnt_type);
        std::string mountPoint(entry->mnt_dir);
        
        // Система фильтрации файловых систем
        // 1. Пропускаем все стандартные виртуальные ФС
        if (fstype == "proc" || fstype == "sysfs" || fstype == "devpts" || 
            fstype == "tmpfs" || fstype == "cgroup" || fstype == "pstore" ||
            fstype == "securityfs" || fstype == "devtmpfs" || fstype == "debugfs" ||
            fstype == "hugetlbfs" || fstype == "mqueue" || fstype == "fusectl" ||
            fstype.find("fuse") != std::string::npos) {
            continue;
        }
        
        // 2. Пропускаем специфические системные разделы, которые могут вызвать лишние оповещения
        std::string device(entry->mnt_fsname);
        
        if (mountPoint.find("/etc/nixmodules") != std::string::npos ||
            mountPoint.find("/mnt/nixmodules") != std::string::npos ||
            mountPoint.find("/nix/store") != std::string::npos ||
            mountPoint.find("/nix") != std::string::npos ||
            mountPoint.find("/run/user") != std::string::npos ||
            device.find("tmpfs") != std::string::npos ||
            device.find("overlay") != std::string::npos) {
            continue;
        }
        
        // 3. Добавляем только разделы размером более 100 МБ
        struct statvfs stat;
        if (statvfs(entry->mnt_dir, &stat) != 0) {
            continue;
        }
        
        DiskInfo info;
        info.device = entry->mnt_fsname;
        info.mountpoint = entry->mnt_dir;
        info.total = stat.f_blocks * stat.f_frsize;
        info.available = stat.f_bavail * stat.f_frsize;
        info.used = (stat.f_blocks - stat.f_bfree) * stat.f_frsize;
        
        // Исключаем слишком маленькие разделы и разделы с 0 размером
        const unsigned long long MIN_SIZE = 100 * 1024 * 1024; // 100 MB
        if (info.total > MIN_SIZE) {
            info.percent = 100.0 * info.used / info.total;
            m_diskInfo.push_back(info);
            
            // Проверяем, есть ли история для этого раздела
            auto it = m_diskHistory.find(info.mountpoint);
            if (it == m_diskHistory.end()) {
                // Создаем новую историю для этого раздела
                std::size_t historySize = 600; // 10 минут с 1-секундным интервалом
                m_diskHistory[info.mountpoint] = std::make_unique<HistoryData>(historySize);
            }
        }
    }
    
    endmntent(mtab);
    return true;
}
