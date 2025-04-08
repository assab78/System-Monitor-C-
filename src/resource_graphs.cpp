#include "resource_graphs.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <sstream>
#include <iomanip>

ResourceGraph::ResourceGraph()
    : m_data(nullptr),
      m_colorR(0.0),
      m_colorG(0.7),
      m_colorB(0.9),
      m_title("Resource Usage")
{
    // Create drawing area widget
    m_drawingArea = gtk_drawing_area_new();
    gtk_widget_set_size_request(m_drawingArea, 300, 150);
    
    // Connect draw signal
    g_signal_connect(G_OBJECT(m_drawingArea), "draw", G_CALLBACK(drawCallback), this);
}

ResourceGraph::~ResourceGraph() {
    // Default destructor
}

GtkWidget* ResourceGraph::getWidget() {
    return m_drawingArea;
}

void ResourceGraph::setDataSource(HistoryData* data) {
    m_data = data;
}

void ResourceGraph::setColor(double r, double g, double b) {
    m_colorR = r;
    m_colorG = g;
    m_colorB = b;
}

void ResourceGraph::setTitle(const std::string& title) {
    m_title = title;
}

void ResourceGraph::redraw() {
    if (m_drawingArea && gtk_widget_get_realized(m_drawingArea)) {
        gtk_widget_queue_draw(m_drawingArea);
    }
}

gboolean ResourceGraph::drawCallback(GtkWidget* widget, cairo_t* cr, gpointer data) {
    ResourceGraph* graph = static_cast<ResourceGraph*>(data);
    int width = gtk_widget_get_allocated_width(widget);
    int height = gtk_widget_get_allocated_height(widget);
    
    graph->draw(cr, width, height);
    
    return FALSE;
}

// Реализация класса DiskGraphPanel

DiskGraphPanel::DiskGraphPanel(const std::string& name, const std::string& device, ResourceGraph* graph)
    : m_name(name),
      m_device(device),
      m_graph(graph)
{
    // Создаем основной контейнер
    m_mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(m_mainBox), 5);
    
    // Добавляем метку с информацией
    std::stringstream labelText;
    labelText << name << " (" << device << ")";
    m_infoLabel = gtk_label_new(labelText.str().c_str());
    gtk_widget_set_halign(m_infoLabel, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(m_mainBox), m_infoLabel, FALSE, FALSE, 0);
    
    // Добавляем график
    gtk_box_pack_start(GTK_BOX(m_mainBox), graph->getWidget(), TRUE, TRUE, 0);
    
    // Показываем все виджеты
    gtk_widget_show_all(m_mainBox);
}

DiskGraphPanel::~DiskGraphPanel() {
    // График должен быть удален в другом месте
}

GtkWidget* DiskGraphPanel::getWidget() {
    return m_mainBox;
}

void DiskGraphPanel::updateInfo(double usedGB, double totalGB, double usagePercent) {
    std::stringstream labelText;
    labelText << m_name << " (" << m_device << "): " 
              << std::fixed << std::setprecision(1) << usedGB 
              << " ГБ / " << std::fixed << std::setprecision(1) << totalGB 
              << " ГБ (" << std::fixed << std::setprecision(1) << usagePercent << "%)";
    gtk_label_set_text(GTK_LABEL(m_infoLabel), labelText.str().c_str());
}

ResourceGraph* DiskGraphPanel::getGraph() {
    return m_graph;
}

void ResourceGraph::draw(cairo_t* cr, int width, int height) {
    // Clear background
    cairo_set_source_rgb(cr, 0.15, 0.15, 0.15);
    cairo_paint(cr);
    
    // Draw border
    cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
    cairo_set_line_width(cr, 1);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_stroke(cr);
    
    // Draw title
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 12);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_text_extents_t extents;
    cairo_text_extents(cr, m_title.c_str(), &extents);
    cairo_move_to(cr, (width - extents.width) / 2, 15);
    cairo_show_text(cr, m_title.c_str());
    
    // If no data, show a message and return
    if (!m_data || m_data->getSize() == 0) {
        cairo_set_font_size(cr, 10);
        const char* msg = "No data available";
        cairo_text_extents(cr, msg, &extents);
        cairo_move_to(cr, (width - extents.width) / 2, height / 2);
        cairo_show_text(cr, msg);
        return;
    }
    
    // Get data samples
    std::vector<double> samples = m_data->getSamples();
    
    // Calculate graph dimensions
    double graphTop = 25;
    double graphBottom = height - 25;
    double graphLeft = 40;
    double graphRight = width - 10;
    double graphHeight = graphBottom - graphTop;
    double graphWidth = graphRight - graphLeft;
    
    // Draw Y-axis scale
    cairo_set_font_size(cr, 9);
    cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
    
    // Draw horizontal grid lines and labels
    for (int i = 0; i <= 10; i++) {
        double y = graphBottom - (i * graphHeight / 10);
        double value = i * 10.0; // 0% to 100%
        
        // Draw grid line
        cairo_set_source_rgba(cr, 0.3, 0.3, 0.3, 0.5);
        cairo_move_to(cr, graphLeft, y);
        cairo_line_to(cr, graphRight, y);
        cairo_stroke(cr);
        
        // Draw label
        cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
        std::string label = std::to_string(static_cast<int>(value)) + "%";
        cairo_text_extents(cr, label.c_str(), &extents);
        cairo_move_to(cr, graphLeft - extents.width - 5, y + extents.height / 2);
        cairo_show_text(cr, label.c_str());
    }
    
    // Draw X-axis (time)
    cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
    cairo_move_to(cr, graphLeft, graphBottom);
    cairo_line_to(cr, graphRight, graphBottom);
    cairo_stroke(cr);
    
    // Draw time labels
    std::vector<std::string> timeLabels = {"Now", "1m", "2m", "3m", "4m", "5m"};
    for (size_t i = 0; i < timeLabels.size(); i++) {
        double x = graphRight - (i * graphWidth / (timeLabels.size() - 1));
        cairo_text_extents(cr, timeLabels[i].c_str(), &extents);
        cairo_move_to(cr, x - extents.width / 2, graphBottom + 15);
        cairo_show_text(cr, timeLabels[i].c_str());
    }
    
    // Draw the graph line
    if (samples.size() > 1) {
        // Set color for the graph line
        cairo_set_source_rgb(cr, m_colorR, m_colorG, m_colorB);
        cairo_set_line_width(cr, 2);
        
        // Start the path at the first point
        size_t numPoints = samples.size();
        double x_scale = graphWidth / (static_cast<double>(numPoints) - 1);
        
        // Move to the first point
        double x = graphRight - 0;
        double y = graphBottom - (samples[numPoints - 1] * graphHeight / 100.0);
        cairo_move_to(cr, x, y);
        
        // Draw lines to each subsequent point
        for (size_t i = 1; i < numPoints; i++) {
            x = graphRight - (i * x_scale);
            y = graphBottom - (samples[numPoints - i - 1] * graphHeight / 100.0);
            cairo_line_to(cr, x, y);
        }
        
        // Stroke the path
        cairo_stroke(cr);
        
        // Fill the area under the graph
        cairo_line_to(cr, graphLeft, graphBottom);
        cairo_line_to(cr, graphRight, graphBottom);
        cairo_close_path(cr);
        cairo_set_source_rgba(cr, m_colorR, m_colorG, m_colorB, 0.2);
        cairo_fill(cr);
    }
    
    // Draw the current value
    if (!samples.empty()) {
        std::string valueText = std::to_string(static_cast<int>(samples.back())) + "%";
        cairo_set_font_size(cr, 14);
        cairo_set_source_rgb(cr, m_colorR, m_colorG, m_colorB);
        cairo_text_extents(cr, valueText.c_str(), &extents);
        cairo_move_to(cr, graphRight - extents.width - 5, graphTop + 15);
        cairo_show_text(cr, valueText.c_str());
    }
}
