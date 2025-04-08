#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include <string>
#include <map>
#include <chrono>
#include <libnotify/notify.h>

// Типы ресурсов для группировки уведомлений
enum class ResourceType {
    CPU,
    Memory,
    Disk,
    Other
};

class NotificationManager {
public:
    NotificationManager();
    ~NotificationManager();
    
    bool initialize();
    
    // Send a notification with given title and message
    void sendNotification(const std::string& title, const std::string& message, NotifyUrgency urgency = NOTIFY_URGENCY_NORMAL);
    
    // Send a notification for a specific resource type
    void sendResourceNotification(ResourceType resourceType, const std::string& title, const std::string& message, NotifyUrgency urgency = NOTIFY_URGENCY_NORMAL);
    
    // Check if a notification with the same category has been sent recently
    bool wasRecentlySent(const std::string& category, int cooldownSeconds = 60);
    
    // Check if a notification for a specific resource type has been sent recently
    bool wasResourceRecentlySent(ResourceType resourceType, int cooldownSeconds = 60);

private:
    bool m_initialized;
    
    // Map to track when notifications were last sent for each category
    std::map<std::string, std::chrono::time_point<std::chrono::steady_clock>> m_lastNotificationTimes;
    
    // Map to track when notifications were last sent for each resource type
    std::map<ResourceType, std::chrono::time_point<std::chrono::steady_clock>> m_lastResourceNotificationTimes;
};

#endif // NOTIFICATION_MANAGER_H
