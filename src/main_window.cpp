#include "main_window.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <vector>

MainWindow::MainWindow()
    : m_window(nullptr),
      m_updateTimerId(0)
{
    // Create components
    m_settings = std::make_unique<Settings>();
    m_resourceMonitor = std::make_unique<ResourceMonitor>(m_settings.get());
    m_notificationManager = std::make_unique<NotificationManager>();
}

MainWindow::~MainWindow() {
    // Stop the update timer
    if (m_updateTimerId > 0) {
        g_source_remove(m_updateTimerId);
        m_updateTimerId = 0;
    }
    
    // Clean up resource graphs for CPU and memory
    for (auto* graph : m_resourceGraphs) {
        delete graph;
    }
    m_resourceGraphs.clear();
    
    // Clean up disk resource graphs
    for (auto& pair : m_diskGraphs) {
        delete pair.second;
    }
    m_diskGraphs.clear();
    
    // Clean up disk graph panels
    for (auto& pair : m_diskPanels) {
        delete pair.second;
    }
    m_diskPanels.clear();
}

bool MainWindow::initialize() {
    // Initialize core components
    if (!m_notificationManager->initialize()) {
        std::cerr << "Failed to initialize notification manager" << std::endl;
        return false;
    }
    
    // Load settings
    m_settings->load();
    
    if (!m_resourceMonitor->initialize()) {
        std::cerr << "Failed to initialize resource monitor" << std::endl;
        return false;
    }
    
    // Setup GUI
    setupWindow();
    
    // Start update timer
    m_updateTimerId = g_timeout_add(m_settings->getUpdateInterval(),
                                    onUpdateTimer, this);
    
    return true;
}

void MainWindow::setupWindow() {
    // Create the main window
    m_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(m_window), "Монитор системных ресурсов");
    gtk_window_set_default_size(GTK_WINDOW(m_window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(m_window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(m_window), 10);
    
    // Connect window destroy signal
    g_signal_connect(G_OBJECT(m_window), "destroy", G_CALLBACK(onWindowDestroy), this);
    
    // Create a vertical box as the main container
    GtkWidget* mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    
    // Create a notebook (tabbed interface)
    m_notebook = gtk_notebook_new();
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(m_notebook), TRUE);
    
    // Create monitoring tab
    m_monitoringPage = createMonitoringTab();
    gtk_notebook_append_page(GTK_NOTEBOOK(m_notebook),
                             m_monitoringPage,
                             gtk_label_new("Мониторинг"));
    
    // Create settings tab
    m_settingsPage = createSettingsTab();
    gtk_notebook_append_page(GTK_NOTEBOOK(m_notebook),
                             m_settingsPage,
                             gtk_label_new("Настройки"));
    
    // Add the notebook to the main box
    gtk_box_pack_start(GTK_BOX(mainBox), m_notebook, TRUE, TRUE, 0);
    
    // Create status bar
    m_statusBar = gtk_statusbar_new();
    m_statusBarContextId = gtk_statusbar_get_context_id(GTK_STATUSBAR(m_statusBar), "default");
    gtk_statusbar_push(GTK_STATUSBAR(m_statusBar), m_statusBarContextId, "Готов");
    gtk_box_pack_start(GTK_BOX(mainBox), m_statusBar, FALSE, FALSE, 0);
    
    // Add the main box to the window
    gtk_container_add(GTK_CONTAINER(m_window), mainBox);
    
    // Show all widgets
    gtk_widget_show_all(m_window);
}

GtkWidget* MainWindow::createMonitoringTab() {
    // Create a vertical box as the main container for the monitoring tab
    GtkWidget* mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(mainBox), 10);
    
    // Create CPU graph
    ResourceGraph* cpuGraph = new ResourceGraph();
    cpuGraph->setTitle("Использование ЦП");
    cpuGraph->setColor(0.2, 0.7, 1.0);
    cpuGraph->setDataSource(m_resourceMonitor->getCPUHistory());
    m_resourceGraphs.push_back(cpuGraph);
    
    // Create Memory graph
    ResourceGraph* memGraph = new ResourceGraph();
    memGraph->setTitle("Использование памяти");
    memGraph->setColor(0.8, 0.4, 0.2);
    memGraph->setDataSource(m_resourceMonitor->getMemoryHistory());
    m_resourceGraphs.push_back(memGraph);
    
    // Add CPU and memory graphs to a horizontal box
    GtkWidget* topGraphsBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(topGraphsBox), createGraphContainer("ЦП", cpuGraph), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(topGraphsBox), createGraphContainer("Память", memGraph), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(mainBox), topGraphsBox, TRUE, TRUE, 0);
    
    // Create disk usage section
    GtkWidget* diskFrame = gtk_frame_new("Использование дисков");
    GtkWidget* diskBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(diskBox), 10);
    gtk_container_add(GTK_CONTAINER(diskFrame), diskBox);
    
    // We'll add disk graphs dynamically in updateUI
    
    gtk_box_pack_start(GTK_BOX(mainBox), diskFrame, TRUE, TRUE, 0);
    
    return mainBox;
}

GtkWidget* MainWindow::createSettingsTab() {
    // Create a vertical box as the main container for the settings tab
    GtkWidget* mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(mainBox), 10);
    
    // Create a grid for the settings
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    
    // CPU threshold setting
    GtkWidget* cpuLabel = gtk_label_new("Порог загрузки ЦП (%):");
    gtk_widget_set_halign(cpuLabel, GTK_ALIGN_START);
    
    m_cpuThresholdScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 50, 100, 1);
    gtk_scale_set_value_pos(GTK_SCALE(m_cpuThresholdScale), GTK_POS_RIGHT);
    gtk_range_set_value(GTK_RANGE(m_cpuThresholdScale), m_settings->getCPUThreshold());
    g_signal_connect(G_OBJECT(m_cpuThresholdScale), "value-changed",
                     G_CALLBACK(onCPUThresholdChanged), this);
    
    gtk_grid_attach(GTK_GRID(grid), cpuLabel, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), m_cpuThresholdScale, 1, 0, 1, 1);
    
    // Memory threshold setting
    GtkWidget* memLabel = gtk_label_new("Порог использования памяти (%):");
    gtk_widget_set_halign(memLabel, GTK_ALIGN_START);
    
    m_memoryThresholdScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 50, 100, 1);
    gtk_scale_set_value_pos(GTK_SCALE(m_memoryThresholdScale), GTK_POS_RIGHT);
    gtk_range_set_value(GTK_RANGE(m_memoryThresholdScale), m_settings->getMemoryThreshold());
    g_signal_connect(G_OBJECT(m_memoryThresholdScale), "value-changed",
                     G_CALLBACK(onMemoryThresholdChanged), this);
    
    gtk_grid_attach(GTK_GRID(grid), memLabel, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), m_memoryThresholdScale, 1, 1, 1, 1);
    
    // Disk threshold setting
    GtkWidget* diskLabel = gtk_label_new("Порог заполнения диска (%):");
    gtk_widget_set_halign(diskLabel, GTK_ALIGN_START);
    
    m_diskThresholdScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 70, 100, 1);
    gtk_scale_set_value_pos(GTK_SCALE(m_diskThresholdScale), GTK_POS_RIGHT);
    gtk_range_set_value(GTK_RANGE(m_diskThresholdScale), m_settings->getDiskThreshold());
    g_signal_connect(G_OBJECT(m_diskThresholdScale), "value-changed",
                     G_CALLBACK(onDiskThresholdChanged), this);
    
    gtk_grid_attach(GTK_GRID(grid), diskLabel, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), m_diskThresholdScale, 1, 2, 1, 1);
    
    // Notification cooldown setting
    GtkWidget* cooldownLabel = gtk_label_new("Задержка между уведомлениями (сек.):");
    gtk_widget_set_halign(cooldownLabel, GTK_ALIGN_START);
    
    m_notificationCooldownSpinner = gtk_spin_button_new_with_range(10, 300, 10);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_notificationCooldownSpinner),
                             m_settings->getNotificationCooldown());
    g_signal_connect(G_OBJECT(m_notificationCooldownSpinner), "value-changed",
                     G_CALLBACK(onNotificationCooldownChanged), this);
    
    gtk_grid_attach(GTK_GRID(grid), cooldownLabel, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), m_notificationCooldownSpinner, 1, 3, 1, 1);
    
    // Add the grid to the main box
    gtk_box_pack_start(GTK_BOX(mainBox), grid, FALSE, FALSE, 0);
    
    // Add a separator
    gtk_box_pack_start(GTK_BOX(mainBox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
    
    // Button box
    GtkWidget* buttonBox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(buttonBox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing(GTK_BOX(buttonBox), 10);
    
    // Save button
    GtkWidget* saveButton = gtk_button_new_with_label("Сохранить настройки");
    g_signal_connect(G_OBJECT(saveButton), "clicked",
                     G_CALLBACK(onSaveSettingsClicked), this);
    gtk_container_add(GTK_CONTAINER(buttonBox), saveButton);
    
    // Reset button
    GtkWidget* resetButton = gtk_button_new_with_label("Сбросить по умолчанию");
    g_signal_connect(G_OBJECT(resetButton), "clicked",
                     G_CALLBACK(onResetSettingsClicked), this);
    gtk_container_add(GTK_CONTAINER(buttonBox), resetButton);
    
    gtk_box_pack_start(GTK_BOX(mainBox), buttonBox, FALSE, FALSE, 0);
    
    return mainBox;
}

GtkWidget* MainWindow::createGraphContainer(const std::string& label, ResourceGraph* graph) {
    GtkWidget* frame = gtk_frame_new(label.c_str());
    GtkWidget* container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(container), 10);
    
    gtk_box_pack_start(GTK_BOX(container), graph->getWidget(), TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(frame), container);
    
    return frame;
}

gboolean MainWindow::onUpdateTimer(gpointer user_data) {
    MainWindow* window = static_cast<MainWindow*>(user_data);
    window->m_resourceMonitor->update();
    window->updateUI();
    
    // Continue the timer
    return TRUE;
}

void MainWindow::updateUI() {
    // Update existing CPU and memory graphs
    for (auto* graph : m_resourceGraphs) {
        graph->redraw();
    }
    
    // Находим контейнер для графиков дисков
    GtkWidget* diskFrame = gtk_notebook_get_nth_page(GTK_NOTEBOOK(m_notebook), 0);
    if (diskFrame) {
        GList* children = gtk_container_get_children(GTK_CONTAINER(diskFrame));
        if (children) {
            GtkWidget* mainBox = reinterpret_cast<GtkWidget*>(children->data);
            g_list_free(children);
            
            // Проверяем, есть ли контейнер для дисков
            children = gtk_container_get_children(GTK_CONTAINER(mainBox));
            if (children && g_list_length(children) >= 3) {
                // Третий элемент - это фрейм с дисками
                GtkWidget* diskFrame = reinterpret_cast<GtkWidget*>(g_list_nth_data(children, 2));
                
                // Получаем данные о дисках
                const std::vector<DiskInfo>& diskInfo = m_resourceMonitor->getDiskInfo();
                
                // Получаем контейнер внутри фрейма
                GList* frameChildren = gtk_container_get_children(GTK_CONTAINER(diskFrame));
                if (frameChildren) {
                    GtkWidget* diskBox = reinterpret_cast<GtkWidget*>(frameChildren->data);
                    g_list_free(frameChildren);
                    
                    // Очищаем текущие виджеты
                    GList* diskBoxChildren = gtk_container_get_children(GTK_CONTAINER(diskBox));
                    if (diskBoxChildren) {
                        for (GList* item = diskBoxChildren; item != nullptr; item = item->next) {
                            gtk_widget_destroy(reinterpret_cast<GtkWidget*>(item->data));
                        }
                        g_list_free(diskBoxChildren);
                    }
                    
                    // Добавляем графики для каждого диска
                    if (diskInfo.empty()) {
                        // Если нет доступных дисков, показываем сообщение
                        GtkWidget* infoLabel = gtk_label_new("Нет доступных физических дисков для отображения.");
                        gtk_widget_set_halign(infoLabel, GTK_ALIGN_CENTER);
                        gtk_widget_set_valign(infoLabel, GTK_ALIGN_CENTER);
                        gtk_box_pack_start(GTK_BOX(diskBox), infoLabel, TRUE, TRUE, 10);
                    } else {
                        // Создаем список существующих дисков для отслеживания удаленных дисков
                        std::set<std::string> currentDisks;
                        
                        for (const auto& disk : diskInfo) {
                            currentDisks.insert(disk.mountpoint);
                            
                            // Создаем новый граф, если его еще нет
                            if (m_diskGraphs.find(disk.mountpoint) == m_diskGraphs.end()) {
                                ResourceGraph* diskGraph = new ResourceGraph();
                                std::string title = "Диск: " + disk.mountpoint;
                                diskGraph->setTitle(title);
                                
                                // Устанавливаем разные цвета для разных дисков
                                // Используем хеш от имени диска для получения уникального цвета
                                size_t hash = std::hash<std::string>{}(disk.mountpoint);
                                double r = 0.3 + (hash % 100) / 200.0;
                                double g = 0.4 + ((hash / 100) % 100) / 200.0;
                                double b = 0.5 + ((hash / 10000) % 100) / 200.0;
                                diskGraph->setColor(r, g, b);
                                
                                // Устанавливаем источник данных
                                diskGraph->setDataSource(m_resourceMonitor->getDiskHistory(disk.mountpoint));
                                m_diskGraphs[disk.mountpoint] = diskGraph;
                            }
                            
                            // Создаем или обновляем панель графика диска
                            double totalGB = disk.total / (1024.0 * 1024.0 * 1024.0);
                            double usedGB = disk.used / (1024.0 * 1024.0 * 1024.0);
                            
                            if (m_diskPanels.find(disk.mountpoint) == m_diskPanels.end()) {
                                // Создаем новую панель
                                DiskGraphPanel* panel = new DiskGraphPanel(
                                    disk.mountpoint,
                                    disk.device,
                                    m_diskGraphs[disk.mountpoint]
                                );
                                m_diskPanels[disk.mountpoint] = panel;
                            }
                            
                            // Обновляем информацию о диске
                            m_diskPanels[disk.mountpoint]->updateInfo(usedGB, totalGB, disk.percent);
                            
                            // Обновляем график
                            m_diskGraphs[disk.mountpoint]->redraw();
                            
                            // Добавляем панель в основной контейнер
                            gtk_box_pack_start(GTK_BOX(diskBox), m_diskPanels[disk.mountpoint]->getWidget(), TRUE, TRUE, 5);
                        }
                        
                        // Удаляем диски, которые больше не доступны
                        std::vector<std::string> disksToRemove;
                        for (const auto& pair : m_diskPanels) {
                            if (currentDisks.find(pair.first) == currentDisks.end()) {
                                disksToRemove.push_back(pair.first);
                            }
                        }
                        
                        for (const auto& mountpoint : disksToRemove) {
                            delete m_diskPanels[mountpoint];
                            m_diskPanels.erase(mountpoint);
                            
                            delete m_diskGraphs[mountpoint];
                            m_diskGraphs.erase(mountpoint);
                        }
                    }
                    
                    // Показываем все виджеты
                    gtk_widget_show_all(diskBox);
                }
            }
            g_list_free(children);
        }
    }
    
    // Обновляем строку состояния
    std::stringstream status;
    status << "ЦП: " << std::fixed << std::setprecision(1) << m_resourceMonitor->getCPUUsage() << "% | ";
    
    const MemoryInfo& memInfo = m_resourceMonitor->getMemoryInfo();
    status << "Память: " << std::fixed << std::setprecision(1) << memInfo.percent << "% | ";
    
    double totalGB = memInfo.total / (1024.0 * 1024.0);
    double usedGB = memInfo.used / (1024.0 * 1024.0);
    status << "Использовано: " << std::fixed << std::setprecision(1) << usedGB 
           << " ГБ / " << std::fixed << std::setprecision(1) << totalGB << " ГБ";
    
    gtk_statusbar_pop(GTK_STATUSBAR(m_statusBar), m_statusBarContextId);
    gtk_statusbar_push(GTK_STATUSBAR(m_statusBar), m_statusBarContextId, status.str().c_str());
    
    // Проверяем пороговые значения и показываем уведомления при необходимости
    std::string message;
    ResourceType resourceType;
    
    if (m_resourceMonitor->checkThresholds(message, resourceType)) {
        // Проверяем, было ли недавно отправлено уведомление для данного типа ресурса
        if (!m_notificationManager->wasResourceRecentlySent(resourceType, m_settings->getNotificationCooldown())) {
            m_notificationManager->sendResourceNotification(
                resourceType, 
                "Предупреждение о ресурсах системы", 
                message, 
                NOTIFY_URGENCY_CRITICAL
            );
        }
    }
}

void MainWindow::onWindowDestroy(GtkWidget* /*widget*/, gpointer data) {
    // Save settings before exit
    MainWindow* window = static_cast<MainWindow*>(data);
    window->m_settings->save();
    
    // Quit the GTK main loop
    gtk_main_quit();
}

void MainWindow::onCPUThresholdChanged(GtkRange* range, gpointer user_data) {
    MainWindow* window = static_cast<MainWindow*>(user_data);
    double value = gtk_range_get_value(range);
    window->m_settings->setCPUThreshold(value);
}

void MainWindow::onMemoryThresholdChanged(GtkRange* range, gpointer user_data) {
    MainWindow* window = static_cast<MainWindow*>(user_data);
    double value = gtk_range_get_value(range);
    window->m_settings->setMemoryThreshold(value);
}

void MainWindow::onDiskThresholdChanged(GtkRange* range, gpointer user_data) {
    MainWindow* window = static_cast<MainWindow*>(user_data);
    double value = gtk_range_get_value(range);
    window->m_settings->setDiskThreshold(value);
}

void MainWindow::onNotificationCooldownChanged(GtkSpinButton* spinner, gpointer user_data) {
    MainWindow* window = static_cast<MainWindow*>(user_data);
    int value = static_cast<int>(gtk_spin_button_get_value(spinner));
    window->m_settings->setNotificationCooldown(value);
}

void MainWindow::onSaveSettingsClicked(GtkButton* /*button*/, gpointer user_data) {
    MainWindow* window = static_cast<MainWindow*>(user_data);
    if (window->m_settings->save()) {
        gtk_statusbar_pop(GTK_STATUSBAR(window->m_statusBar), window->m_statusBarContextId);
        gtk_statusbar_push(GTK_STATUSBAR(window->m_statusBar), window->m_statusBarContextId, "Настройки успешно сохранены");
    } else {
        gtk_statusbar_pop(GTK_STATUSBAR(window->m_statusBar), window->m_statusBarContextId);
        gtk_statusbar_push(GTK_STATUSBAR(window->m_statusBar), window->m_statusBarContextId, "Не удалось сохранить настройки");
    }
}

void MainWindow::onResetSettingsClicked(GtkButton* /*button*/, gpointer user_data) {
    MainWindow* window = static_cast<MainWindow*>(user_data);
    
    // Reset to default values
    window->m_settings->setCPUThreshold(85.0); // Новое значение по умолчанию
    window->m_settings->setMemoryThreshold(85.0); // Новое значение по умолчанию
    window->m_settings->setDiskThreshold(90.0);
    window->m_settings->setNotificationCooldown(300); // Новое значение по умолчанию (5 минут)
    
    // Update UI controls
    gtk_range_set_value(GTK_RANGE(window->m_cpuThresholdScale), 85.0);
    gtk_range_set_value(GTK_RANGE(window->m_memoryThresholdScale), 85.0);
    gtk_range_set_value(GTK_RANGE(window->m_diskThresholdScale), 90.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(window->m_notificationCooldownSpinner), 300);
    
    gtk_statusbar_pop(GTK_STATUSBAR(window->m_statusBar), window->m_statusBarContextId);
    gtk_statusbar_push(GTK_STATUSBAR(window->m_statusBar), window->m_statusBarContextId, "Настройки сброшены по умолчанию");
}
