#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include "resource_monitor.h"
#include "notification_manager.h"
#include "settings.h"
#include "resource_graphs.h"

class MainWindow {
public:
    MainWindow();
    ~MainWindow();
    
    bool initialize();

private:
    GtkWidget* m_window;
    GtkWidget* m_notebook;
    
    // Resource monitoring tab
    GtkWidget* m_monitoringPage;
    std::vector<ResourceGraph*> m_resourceGraphs;
    std::map<std::string, ResourceGraph*> m_diskGraphs; // Графики для дисков
    std::map<std::string, DiskGraphPanel*> m_diskPanels; // Панели с графиками дисков
    
    // Settings tab
    GtkWidget* m_settingsPage;
    GtkWidget* m_cpuThresholdScale;
    GtkWidget* m_memoryThresholdScale;
    GtkWidget* m_diskThresholdScale;
    GtkWidget* m_notificationCooldownSpinner;
    
    // Status bar
    GtkWidget* m_statusBar;
    guint m_statusBarContextId;
    
    // Core components
    std::unique_ptr<Settings> m_settings;
    std::unique_ptr<ResourceMonitor> m_resourceMonitor;
    std::unique_ptr<NotificationManager> m_notificationManager;
    
    // Timer for periodic updates
    guint m_updateTimerId;
    
    // Initialize the main window
    void setupWindow();
    
    // Create the resource monitoring tab
    GtkWidget* createMonitoringTab();
    
    // Create the settings tab
    GtkWidget* createSettingsTab();
    
    // Create a graph container with label
    GtkWidget* createGraphContainer(const std::string& label, ResourceGraph* graph);
    
    // Update timer callback
    static gboolean onUpdateTimer(gpointer user_data);
    
    // Update UI with current resource usage
    void updateUI();
    
    // Window destruction callback
    static void onWindowDestroy(GtkWidget* widget, gpointer data);
    
    // Settings changed callbacks
    static void onCPUThresholdChanged(GtkRange* range, gpointer user_data);
    static void onMemoryThresholdChanged(GtkRange* range, gpointer user_data);
    static void onDiskThresholdChanged(GtkRange* range, gpointer user_data);
    static void onNotificationCooldownChanged(GtkSpinButton* spinner, gpointer user_data);
    
    // Button callbacks
    static void onSaveSettingsClicked(GtkButton* button, gpointer user_data);
    static void onResetSettingsClicked(GtkButton* button, gpointer user_data);
};

#endif // MAIN_WINDOW_H
