
#include "browser_window.hpp"

#include "bacus-core/browser_config.hpp"

#include <string>

namespace bacus::app {

BrowserWindow::BrowserWindow(GtkApplication* application) {
    window_ = gtk_application_window_new(application);

    gtk_window_set_title(GTK_WINDOW(window_), bacus::core::BROWSER_NAME.data());

    gtk_window_set_default_size(GTK_WINDOW(window_), bacus::core::DEFAULT_WINDOW_WIDTH,
                                bacus::core::DEFAULT_WINDOW_HEIGHT);

    GtkWidget* root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

    gtk_container_set_border_width(GTK_CONTAINER(root), 12);

    gtk_container_add(GTK_CONTAINER(window_), root);

    // Navigation toolbar
    GtkWidget* toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);

    gtk_box_pack_start(GTK_BOX(root), toolbar, FALSE, FALSE, 0);

    address_entry_ = gtk_entry_new();

    gtk_entry_set_placeholder_text(GTK_ENTRY(address_entry_), "Escribe una dirección web...");

    gtk_entry_set_text(GTK_ENTRY(address_entry_), bacus::core::DEFAULT_HOME.data());

    gtk_widget_set_hexpand(address_entry_, TRUE);

    gtk_box_pack_start(GTK_BOX(toolbar), address_entry_, TRUE, TRUE, 0);

    GtkWidget* navigate_button = gtk_button_new_with_label("Ir");

    gtk_box_pack_start(GTK_BOX(toolbar), navigate_button, FALSE, FALSE, 0);

    g_signal_connect(navigate_button, "clicked", G_CALLBACK(on_navigate_clicked), this);

    g_signal_connect(address_entry_, "activate", G_CALLBACK(on_address_activate), this);

    // Page area
    page_label_ = gtk_label_new("Bienvenido a Bacus Explorer\n\n"
                                "El motor Chromium se integrará en una próxima etapa.");

    gtk_label_set_justify(GTK_LABEL(page_label_), GTK_JUSTIFY_CENTER);

    gtk_widget_set_hexpand(page_label_, TRUE);
    gtk_widget_set_vexpand(page_label_, TRUE);

    gtk_box_pack_start(GTK_BOX(root), page_label_, TRUE, TRUE, 0);

    // Status bar
    status_label_ = gtk_label_new("Bacus Explorer 0.1.0 | Motor web pendiente de integración");

    gtk_widget_set_halign(status_label_, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(root), status_label_, FALSE, FALSE, 0);
}

void BrowserWindow::show() {
    gtk_widget_show_all(window_);
}

void BrowserWindow::update_status(const char* message) {
    gtk_label_set_text(GTK_LABEL(status_label_), message);
}

void BrowserWindow::navigate() {
    const char* address = gtk_entry_get_text(GTK_ENTRY(address_entry_));

    if (address == nullptr || *address == '\0') {
        update_status("Escribe una dirección antes de continuar.");
        return;
    }

    // This first version intentionally does not load remote content.
    // Chromium integration will provide actual navigation later.

    std::string message = "Dirección preparada: ";

    message += address;

    gtk_label_set_text(GTK_LABEL(page_label_), message.c_str());

    update_status("La navegación web estará disponible al integrar Chromium.");
}

void BrowserWindow::on_navigate_clicked(GtkButton*, gpointer user_data) {
    auto* browser = static_cast<BrowserWindow*>(user_data);
    browser->navigate();
}

void BrowserWindow::on_address_activate(GtkEntry*, gpointer user_data) {
    auto* browser = static_cast<BrowserWindow*>(user_data);
    browser->navigate();
}

} // namespace bacus::app
