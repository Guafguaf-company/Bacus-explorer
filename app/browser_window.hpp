#pragma once

#include <gtk/gtk.h>

namespace bacus::app {

class BrowserWindow {
  public:
    explicit BrowserWindow(GtkApplication* application);
    ~BrowserWindow() = default;

    BrowserWindow(const BrowserWindow&) = delete;
    BrowserWindow& operator=(const BrowserWindow&) = delete;

    void show();

  private:
    GtkWidget* window_ = nullptr;
    GtkWidget* address_entry_ = nullptr;
    GtkWidget* notebook_ = nullptr;
    GtkWidget* status_label_ = nullptr;

    GtkWidget* create_tab(const char* title, const char* address);
    void add_tab();
    void navigate();
    void update_status(const char* message);

    static void on_new_tab_clicked(GtkButton* button, gpointer user_data);
    static void on_navigate_clicked(GtkButton* button, gpointer user_data);
    static void on_address_activate(GtkEntry* entry, gpointer user_data);
    static void on_switch_page(
        GtkNotebook* notebook,
        GtkWidget* page,
        guint page_number,
        gpointer user_data
    );
};

} // namespace bacus::app
