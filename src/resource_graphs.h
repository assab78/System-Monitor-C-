#ifndef RESOURCE_GRAPHS_H
#define RESOURCE_GRAPHS_H

#include <gtk/gtk.h>
#include <string>
#include "history_data.h"

class ResourceGraph {
public:
    ResourceGraph();
    ~ResourceGraph();
    
    // Create and get the graph widget
    GtkWidget* getWidget();
    
    // Set the data source
    void setDataSource(HistoryData* data);
    
    // Set graph color
    void setColor(double r, double g, double b);
    
    // Set title
    void setTitle(const std::string& title);
    
    // Force redraw
    void redraw();
    
private:
    GtkWidget* m_drawingArea;
    HistoryData* m_data;
    double m_colorR;
    double m_colorG;
    double m_colorB;
    std::string m_title;
    
    // Draw callback for the drawing area
    static gboolean drawCallback(GtkWidget* widget, cairo_t* cr, gpointer data);
    
    // Draw the graph
    void draw(cairo_t* cr, int width, int height);
};

// Класс для отображения графика использования диска с дополнительной информацией
class DiskGraphPanel {
public:
    DiskGraphPanel(const std::string& name, const std::string& device, ResourceGraph* graph);
    ~DiskGraphPanel();
    
    // Получить основной виджет
    GtkWidget* getWidget();
    
    // Обновить информацию о диске
    void updateInfo(double usedGB, double totalGB, double usagePercent);
    
    // Получить график
    ResourceGraph* getGraph();
    
private:
    GtkWidget* m_mainBox;
    GtkWidget* m_infoLabel;
    std::string m_name;
    std::string m_device;
    ResourceGraph* m_graph;
};

#endif // RESOURCE_GRAPHS_H
