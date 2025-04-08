#include <gtk/gtk.h>
#include <iostream>
#include "main_window.h"

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);
    
    // Set application name
    g_set_application_name("System Resource Monitor");
    
    // Create main window
    MainWindow* mainWindow = new MainWindow();
    if (!mainWindow->initialize()) {
        std::cerr << "Failed to initialize main window!" << std::endl;
        delete mainWindow;
        return 1;
    }
    
    // Start the GTK main loop
    gtk_main();
    
    // Clean up
    delete mainWindow;
    
    return 0;
}
