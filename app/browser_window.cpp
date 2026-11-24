#include "browser_window.hpp"

#include "bacus-core/browser_config.hpp"
#include "internal_pages.hpp"

#include <string>

namespace bacus::app {

BrowserWindow::BrowserWindow(GtkApplication* application) {
    window_ = gtk_application_window_new(application);

    gtk_window_set_title(
        GTK_WINDOW(window_),
        bacus::core::BROWSER_NAME.data()
    );

    gtk_window_set_default_size(
        GTK_WINDOW(window_),
        bacus::core::DEFAULT_WINDOW_WIDTH,
        bacus::core::DEFAULT_WINDOW_HEIGHT
    );

    GtkWidget* root = gtk_box_new(
        GTK_ORIENTATION_VERTICAL,
        6
    );

    gtk_container_set_border_width(
        GTK_CONTAINER(root),
        8
    );

    gtk_container_add(
        GTK_CONTAINER(window_),
        root
    );

    // Navigation toolbar
    GtkWidget* toolbar = gtk_box_new(
        GTK_ORIENTATION_HORIZONTAL,
        6
    );

    gtk_box_pack_start(
        GTK_BOX(root),
        toolbar,
        FALSE,
        FALSE,
        0
    );

    GtkWidget* back_button = gtk_button_new_with_label("←");
    GtkWidget* forward_button = gtk_button_new_with_label("→");
    GtkWidget* reload_button = gtk_button_new_with_label("⟳");

    gtk_widget_set_tooltip_text(back_button, "Atrás");
    gtk_widget_set_tooltip_text(forward_button, "Adelante");
    gtk_widget_set_tooltip_text(reload_button, "Recargar");

    gtk_widget_set_sensitive(back_button, FALSE);
    gtk_widget_set_sensitive(forward_button, FALSE);
    gtk_widget_set_sensitive(reload_button, FALSE);

    gtk_box_pack_start(GTK_BOX(toolbar), back_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), forward_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), reload_button, FALSE, FALSE, 0);

    address_entry_ = gtk_entry_new();

    gtk_entry_set_placeholder_text(
        GTK_ENTRY(address_entry_),
        "Escribe una dirección web o about:..."
    );

    gtk_widget_set_hexpand(address_entry_, TRUE);

    gtk_box_pack_start(
        GTK_BOX(toolbar),
        address_entry_,
        TRUE,
        TRUE,
        0
    );

    GtkWidget* navigate_button = gtk_button_new_with_label("Ir");

    gtk_box_pack_start(
        GTK_BOX(toolbar),
        navigate_button,
        FALSE,
        FALSE,
        0
    );

    GtkWidget* new_tab_button = gtk_button_new_with_label("+");

    gtk_widget_set_tooltip_text(
        new_tab_button,
        "Nueva pestaña"
    );

    gtk_box_pack_start(
        GTK_BOX(toolbar),
        new_tab_button,
        FALSE,
        FALSE,
        0
    );

    g_signal_connect(
        navigate_button,
        "clicked",
        G_CALLBACK(on_navigate_clicked),
        this
    );

    g_signal_connect(
        new_tab_button,
        "clicked",
        G_CALLBACK(on_new_tab_clicked),
        this
    );

    g_signal_connect(
        address_entry_,
        "activate",
        G_CALLBACK(on_address_activate),
        this
    );

    // Tab system
    notebook_ = gtk_notebook_new();

    gtk_notebook_set_scrollable(
        GTK_NOTEBOOK(notebook_),
        TRUE
    );

    gtk_widget_set_hexpand(notebook_, TRUE);
    gtk_widget_set_vexpand(notebook_, TRUE);

    gtk_box_pack_start(
        GTK_BOX(root),
        notebook_,
        TRUE,
        TRUE,
        0
    );

    g_signal_connect(
        notebook_,
        "switch-page",
        G_CALLBACK(on_switch_page),
        this
    );

    // Status bar
    status_label_ = gtk_label_new(
        "Bacus Explorer 0.1.0 | Preparado"
    );

    gtk_widget_set_halign(
        status_label_,
        GTK_ALIGN_START
    );

    gtk_box_pack_start(
        GTK_BOX(root),
        status_label_,
        FALSE,
        FALSE,
        0
    );

    add_tab();
}

GtkWidget* BrowserWindow::create_tab(
    const char* title,
    const char* address
) {
    GtkWidget* page = gtk_box_new(
        GTK_ORIENTATION_VERTICAL,
        0
    );

    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);

    g_object_set_data_full(
        G_OBJECT(page),
        "bacus-address",
        g_strdup(address),
        g_free
    );

    GtkWidget* content = create_internal_page(address);

    gtk_box_pack_start(
        GTK_BOX(page),
        content,
        TRUE,
        TRUE,
        0
    );

    GtkWidget* tab_label = gtk_label_new(title);

    gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook_),
        page,
        tab_label
    );

    gtk_widget_show_all(page);
    gtk_widget_show_all(tab_label);

    return page;
}

void BrowserWindow::add_tab() {
    create_tab(
        "Nueva pestaña",
        "about:newtab"
    );

    const gint page_count = gtk_notebook_get_n_pages(
        GTK_NOTEBOOK(notebook_)
    );

    gtk_notebook_set_current_page(
        GTK_NOTEBOOK(notebook_),
        page_count - 1
    );

    update_status("Nueva pestaña creada.");
}

void BrowserWindow::show() {
    gtk_widget_show_all(window_);
}

void BrowserWindow::update_status(const char* message) {
    gtk_label_set_text(
        GTK_LABEL(status_label_),
        message
    );
}

void BrowserWindow::navigate() {
    const char* address = gtk_entry_get_text(
        GTK_ENTRY(address_entry_)
    );

    if (address == nullptr || *address == '\0') {
        update_status("Escribe una dirección antes de continuar.");
        return;
    }

    GtkWidget* page = gtk_notebook_get_nth_page(
        GTK_NOTEBOOK(notebook_),
        gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook_))
    );

    if (page == nullptr) {
        update_status("No hay ninguna pestaña activa.");
        return;
    }

    GList* children = gtk_container_get_children(
        GTK_CONTAINER(page)
    );

    for (GList* item = children; item != nullptr; item = item->next) {
        gtk_widget_destroy(GTK_WIDGET(item->data));
    }

    g_list_free(children);

    g_object_set_data_full(
        G_OBJECT(page),
        "bacus-address",
        g_strdup(address),
        g_free
    );

    GtkWidget* content = create_internal_page(address);

    gtk_box_pack_start(
        GTK_BOX(page),
        content,
        TRUE,
        TRUE,
        0
    );

    gtk_widget_show_all(page);

    std::string message = "Dirección interna: ";
    message += address;

    update_status(message.c_str());
}

void BrowserWindow::on_new_tab_clicked(
    GtkButton*,
    gpointer user_data
) {
    auto* browser = static_cast<BrowserWindow*>(user_data);
    browser->add_tab();
}

void BrowserWindow::on_navigate_clicked(
    GtkButton*,
    gpointer user_data
) {
    auto* browser = static_cast<BrowserWindow*>(user_data);
    browser->navigate();
}

void BrowserWindow::on_address_activate(
    GtkEntry*,
    gpointer user_data
) {
    auto* browser = static_cast<BrowserWindow*>(user_data);
    browser->navigate();
}

void BrowserWindow::on_switch_page(
    GtkNotebook*,
    GtkWidget* page,
    guint,
    gpointer user_data
) {
    auto* browser = static_cast<BrowserWindow*>(user_data);

    const char* address = static_cast<const char*>(
        g_object_get_data(G_OBJECT(page), "bacus-address")
    );

    gtk_entry_set_text(
        GTK_ENTRY(browser->address_entry_),
        address != nullptr ? address : ""
    );

    browser->update_status("Pestaña activa.");
}

} // namespace bacus::app
