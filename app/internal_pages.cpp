#include "internal_pages.hpp"

#include "bacus-core/browser_config.hpp"

namespace bacus::app {

namespace {

GtkWidget* create_home_page() {
    GtkWidget* root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 18);

    gtk_widget_set_halign(root, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(root, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(root, TRUE);
    gtk_widget_set_vexpand(root, TRUE);

    gtk_widget_set_margin_start(root, 32);
    gtk_widget_set_margin_end(root, 32);
    gtk_widget_set_margin_top(root, 32);
    gtk_widget_set_margin_bottom(root, 32);

    GtkWidget* title = gtk_label_new(nullptr);

    gtk_label_set_markup(
        GTK_LABEL(title),
        "<span size='xx-large' weight='bold'>Bacus Explorer</span>"
    );

    gtk_box_pack_start(
        GTK_BOX(root),
        title,
        FALSE,
        FALSE,
        0
    );

    GtkWidget* subtitle = gtk_label_new(
        "Tu navegador, tu espacio."
    );

    gtk_box_pack_start(
        GTK_BOX(root),
        subtitle,
        FALSE,
        FALSE,
        0
    );

    GtkWidget* welcome = gtk_label_new(
        "Bienvenido a Bacus Explorer 0.1.0"
    );

    gtk_box_pack_start(
        GTK_BOX(root),
        welcome,
        FALSE,
        FALSE,
        12
    );

    GtkWidget* description = gtk_label_new(
        "Escribe una dirección web en la barra superior "
        "para comenzar.\n\n"
        "El motor Chromium estará disponible en una "
        "próxima etapa."
    );

    gtk_label_set_justify(
        GTK_LABEL(description),
        GTK_JUSTIFY_CENTER
    );

    gtk_label_set_line_wrap(
        GTK_LABEL(description),
        TRUE
    );

    gtk_box_pack_start(
        GTK_BOX(root),
        description,
        FALSE,
        FALSE,
        0
    );

    GtkWidget* company = gtk_label_new(
        bacus::core::COMPANY.data()
    );

    gtk_box_pack_start(
        GTK_BOX(root),
        company,
        FALSE,
        FALSE,
        12
    );

    return root;
}

GtkWidget* create_blank_page() {
    GtkWidget* page = gtk_box_new(
        GTK_ORIENTATION_VERTICAL,
        0
    );

    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);

    return page;
}

GtkWidget* create_error_page(const std::string& uri) {
    GtkWidget* root = gtk_box_new(
        GTK_ORIENTATION_VERTICAL,
        12
    );

    gtk_widget_set_halign(root, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(root, GTK_ALIGN_CENTER);

    GtkWidget* title = gtk_label_new(
        "Página interna no disponible"
    );

    gtk_box_pack_start(
        GTK_BOX(root),
        title,
        FALSE,
        FALSE,
        0
    );

    GtkWidget* address = gtk_label_new(uri.c_str());

    gtk_box_pack_start(
        GTK_BOX(root),
        address,
        FALSE,
        FALSE,
        0
    );

    GtkWidget* description = gtk_label_new(
        "Esta página todavía no está implementada."
    );

    gtk_box_pack_start(
        GTK_BOX(root),
        description,
        FALSE,
        FALSE,
        0
    );

    return root;
}

} // namespace

GtkWidget* create_internal_page(const std::string& uri) {
    if (uri == "about:blank") {
        return create_blank_page();
    }

    if (uri == "about:home" || uri == "about:newtab") {
        return create_home_page();
    }

    if (uri.rfind("about:", 0) == 0) {
        return create_error_page(uri);
    }

    return create_error_page(uri);
}

} // namespace bacus::app
