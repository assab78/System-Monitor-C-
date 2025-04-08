#include "notification_manager.h"
#include <iostream>

NotificationManager::NotificationManager() : m_initialized(false) {
    // Default constructor
}

NotificationManager::~NotificationManager() {
    if (m_initialized) {
        notify_uninit();
    }
}

bool NotificationManager::initialize() {
    if (!notify_init("System Resource Monitor")) {
        std::cerr << "Failed to initialize libnotify - notifications will be disabled" << std::endl;
        // We return true anyway to allow the app to continue running without notifications
        return true;
    }
    
    m_initialized = true;
    return true;
}

void NotificationManager::sendNotification(const std::string& title, const std::string& message, NotifyUrgency urgency) {
    // Always display the notification message in the console
    std::cout << "NOTIFICATION [" << title << "]: " << message << std::endl;
    
    if (!m_initialized) {
        std::cerr << "Notification manager not initialized" << std::endl;
        return;
    }
    
    // Try to create and show a system notification
    // If it fails, we've already displayed it in the console
    NotifyNotification* notification = nullptr;
    
    try {
        notification = notify_notification_new(
            title.c_str(),
            message.c_str(),
            "dialog-warning" // Icon name
        );
        
        if (notification) {
            // Set urgency level
            notify_notification_set_urgency(notification, urgency);
            
            // Show notification
            GError* error = nullptr;
            if (!notify_notification_show(notification, &error)) {
                // Just log it, but don't spam the console with errors
                if (error) {
                    g_error_free(error);
                }
            }
            
            // Free the notification object
            g_object_unref(G_OBJECT(notification));
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in sendNotification: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in sendNotification" << std::endl;
    }
    
    // Update last sent time for the category based on title
    m_lastNotificationTimes[title] = std::chrono::steady_clock::now();
}

void NotificationManager::sendResourceNotification(ResourceType resourceType, const std::string& title, const std::string& message, NotifyUrgency urgency) {
    // Record the time for the resource type
    m_lastResourceNotificationTimes[resourceType] = std::chrono::steady_clock::now();
    
    // Send the actual notification
    sendNotification(title, message, urgency);
}

bool NotificationManager::wasRecentlySent(const std::string& category, int cooldownSeconds) {
    auto it = m_lastNotificationTimes.find(category);
    if (it == m_lastNotificationTimes.end()) {
        return false;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
    
    return elapsed < cooldownSeconds;
}

bool NotificationManager::wasResourceRecentlySent(ResourceType resourceType, int cooldownSeconds) {
    auto it = m_lastResourceNotificationTimes.find(resourceType);
    if (it == m_lastResourceNotificationTimes.end()) {
        return false;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - it->second).count();
    
    return elapsed < cooldownSeconds;
}
