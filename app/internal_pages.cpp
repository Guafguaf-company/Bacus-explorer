
#include "internal_pages.hpp"
#include "preferences.hpp"

#include "bacus-core/browser_config.hpp"

#include <gtk/gtk.h>

#include <string>

namespace bacus::app {

namespace {

constexpr const char* DEFAULT_CLIMA_URL = "https://climag.duckdns.org";

struct PreferencesWidgets {
    GtkWidget* homepage = nullptr;
    GtkWidget* search_engine = nullptr;
    GtkWidget* theme = nullptr;
    GtkWidget* restore_tabs = nullptr;

    GtkWidget* tracking_protection = nullptr;
    GtkWidget* clear_cookies = nullptr;

    GtkWidget* clima_enabled = nullptr;
    GtkWidget* clima_auto_location = nullptr;
    GtkWidget* clima_url = nullptr;

    GtkWidget* download_directory = nullptr;
    GtkWidget* hardware_acceleration = nullptr;

    GtkWidget* background_image = nullptr;

    GtkWidget* vault_enabled = nullptr;
    GtkWidget* vault_auto_lock = nullptr;

    GtkWidget* extensions_enabled = nullptr;
    GtkWidget* extensions_developer_mode = nullptr;

    GtkWidget* status = nullptr;
};

bool get_boolean(GKeyFile* key_file, const char* group, const char* key, bool fallback) {
    GError* error = nullptr;

    const gboolean value = g_key_file_get_boolean(key_file, group, key, &error);

    if (error != nullptr) {
        g_error_free(error);
        return fallback;
    }

    return value != FALSE;
}

void set_entry(GKeyFile* key_file, const char* group, const char* key, GtkWidget* widget) {
    const char* value = gtk_entry_get_text(GTK_ENTRY(widget));

    g_key_file_set_string(key_file, group, key, value != nullptr ? value : "");
}

void set_check(GKeyFile* key_file, const char* group, const char* key, GtkWidget* widget) {
    g_key_file_set_boolean(key_file, group, key,
                           gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}

void set_combo(GKeyFile* key_file, const char* group, const char* key, GtkWidget* widget) {
    gchar* value = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));

    if (value != nullptr) {
        g_key_file_set_string(key_file, group, key, value);
        g_free(value);
    }
}

void save_preferences(GtkButton*, gpointer user_data) {
    auto* p = static_cast<PreferencesWidgets*>(user_data);

    GKeyFile* key_file = bacus::app::load_preferences();

    set_entry(key_file, "general", "homepage", p->homepage);
    set_combo(key_file, "search", "engine", p->search_engine);
    set_combo(key_file, "appearance", "theme", p->theme);

    set_check(key_file, "general", "restore_tabs", p->restore_tabs);

    set_check(key_file, "privacy", "tracking_protection", p->tracking_protection);

    set_check(key_file, "privacy", "clear_cookies", p->clear_cookies);

    set_check(key_file, "clima_g", "enabled", p->clima_enabled);

    set_check(key_file, "clima_g", "automatic_location", p->clima_auto_location);

    set_entry(key_file, "clima_g", "url", p->clima_url);

    set_entry(key_file, "downloads", "directory", p->download_directory);

    set_check(key_file, "performance", "hardware_acceleration", p->hardware_acceleration);

    set_entry(key_file, "appearance", "background_image", p->background_image);

    set_check(key_file, "vault", "enabled", p->vault_enabled);

    set_check(key_file, "vault", "automatic_lock", p->vault_auto_lock);

    set_check(key_file, "extensions", "enabled", p->extensions_enabled);

    set_check(key_file, "extensions", "developer_mode", p->extensions_developer_mode);

    if (bacus::app::save_preferences(key_file)) {
        gtk_label_set_text(GTK_LABEL(p->status),
                           "Configuración guardada correctamente.");
    } else {
        gtk_label_set_text(GTK_LABEL(p->status),
                           "No se pudo guardar la configuración.");
    }

    g_key_file_unref(key_file);
}

void reset_preferences(GtkButton*, gpointer user_data) {
    auto* p = static_cast<PreferencesWidgets*>(user_data);

    gtk_entry_set_text(GTK_ENTRY(p->homepage), "about:home");

    // Google es el buscador predeterminado.
    gtk_combo_box_set_active(GTK_COMBO_BOX(p->search_engine), 0);

    gtk_combo_box_set_active(GTK_COMBO_BOX(p->theme), 0);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->restore_tabs), TRUE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->tracking_protection), TRUE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->clear_cookies), FALSE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->clima_enabled), TRUE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->clima_auto_location), TRUE);

    gtk_entry_set_text(GTK_ENTRY(p->clima_url), DEFAULT_CLIMA_URL);

    const char* downloads = g_get_user_special_dir(G_USER_DIRECTORY_DOWNLOAD);

    gtk_entry_set_text(GTK_ENTRY(p->download_directory),
                       downloads != nullptr ? downloads : g_get_home_dir());

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->hardware_acceleration), TRUE);

    gtk_entry_set_text(GTK_ENTRY(p->background_image), "");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->vault_enabled), FALSE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->vault_auto_lock), TRUE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->extensions_enabled), TRUE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->extensions_developer_mode), FALSE);

    gtk_label_set_text(GTK_LABEL(p->status), "Valores restaurados. Pulsa Guardar para aplicarlos.");
}

GtkWidget* create_section(const char* title, GtkWidget** grid_output) {
    GtkWidget* frame = gtk_frame_new(title);
    GtkWidget* grid = gtk_grid_new();

    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    gtk_container_set_border_width(GTK_CONTAINER(grid), 12);
    gtk_container_add(GTK_CONTAINER(frame), grid);

    *grid_output = grid;

    return frame;
}

void add_setting(GtkWidget* grid, int row, const char* label, GtkWidget* control) {
    GtkWidget* text = gtk_label_new(label);

    gtk_widget_set_halign(text, GTK_ALIGN_START);
    gtk_widget_set_valign(text, GTK_ALIGN_CENTER);

    gtk_grid_attach(GTK_GRID(grid), text, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), control, 1, row, 1, 1);

    gtk_widget_set_hexpand(control, TRUE);
}

GtkWidget* create_config_page() {
    GKeyFile* key_file = bacus::app::load_preferences();

    auto* p = g_new0(PreferencesWidgets, 1);

    GtkWidget* scrolled = gtk_scrolled_window_new(nullptr, nullptr);

    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scrolled),
        GTK_POLICY_NEVER,
        GTK_POLICY_AUTOMATIC);

    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);

    GtkWidget* root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);

    gtk_container_set_border_width(GTK_CONTAINER(root), 24);

    gtk_container_add(GTK_CONTAINER(scrolled), root);

    GtkWidget* title = gtk_label_new(nullptr);

    gtk_label_set_markup(GTK_LABEL(title), "<span size='xx-large' weight='bold'>"
                                           "Configuración de Bacus Explorer"
                                           "</span>");

    gtk_widget_set_halign(title, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(root), title, FALSE, FALSE, 0);

    GtkWidget* subtitle = gtk_label_new("Administra todas las preferencias del navegador.");

    gtk_widget_set_halign(subtitle, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(root), subtitle, FALSE, FALSE, 0);

    // General
    GtkWidget* general_grid = nullptr;

    GtkWidget* general = create_section("General", &general_grid);

    p->homepage = gtk_entry_new();

    gtk_entry_set_text(GTK_ENTRY(p->homepage),
                       bacus::app::get_preference(key_file, "general", "homepage", "about:home").c_str());

    add_setting(general_grid, 0, "Página de inicio", p->homepage);

    p->restore_tabs = gtk_check_button_new_with_label("Restaurar pestañas anteriores al iniciar");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->restore_tabs),
                                 get_boolean(key_file, "general", "restore_tabs", true));

    gtk_grid_attach(GTK_GRID(general_grid), p->restore_tabs, 0, 1, 2, 1);

    gtk_box_pack_start(GTK_BOX(root), general, FALSE, FALSE, 0);

    // Search
    GtkWidget* search_grid = nullptr;

    GtkWidget* search = create_section("Búsqueda", &search_grid);

    p->search_engine = gtk_combo_box_text_new();

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p->search_engine), "Google");

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p->search_engine), "DuckDuckGo");

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p->search_engine), "Bing");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p->search_engine), "Wikipedia");

    const std::string search_value = bacus::app::get_preference(key_file, "search", "engine", "Google");

    int search_index = 0;

    if (search_value == "DuckDuckGo") {
        search_index = 1;
    } else if (search_value == "Bing") {
        search_index = 2;
    } else if (search_value == "Wikipedia") {
        search_index = 3;
    }

    gtk_combo_box_set_active(GTK_COMBO_BOX(p->search_engine), search_index);

    add_setting(search_grid, 0, "Buscador predeterminado", p->search_engine);

    gtk_box_pack_start(GTK_BOX(root), search, FALSE, FALSE, 0);

    // Appearance
    GtkWidget* appearance_grid = nullptr;

    GtkWidget* appearance = create_section("Apariencia", &appearance_grid);

    p->theme = gtk_combo_box_text_new();

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p->theme), "Sistema");

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p->theme), "Claro");

    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(p->theme), "Oscuro");

    const std::string theme_value = bacus::app::get_preference(key_file, "appearance", "theme", "Sistema");

    int theme_index = 0;

    if (theme_value == "Claro") {
        theme_index = 1;
    } else if (theme_value == "Oscuro") {
        theme_index = 2;
    }

    gtk_combo_box_set_active(GTK_COMBO_BOX(p->theme), theme_index);

    add_setting(appearance_grid, 0, "Tema del navegador", p->theme);

    p->background_image = gtk_entry_new();

    gtk_entry_set_placeholder_text(GTK_ENTRY(p->background_image), "Ruta de una imagen local");

    gtk_entry_set_text(GTK_ENTRY(p->background_image),
                       bacus::app::get_preference(key_file, "appearance", "background_image", "").c_str());

    add_setting(appearance_grid, 1, "Fondo de about:home", p->background_image);

    GtkWidget* background_button = gtk_button_new_with_label("Seleccionar imagen...");

    gtk_grid_attach(GTK_GRID(appearance_grid), background_button, 1, 2, 1, 1);

    g_signal_connect(
        background_button, "clicked", G_CALLBACK(+[](GtkButton*, gpointer data) {
            auto* widgets = static_cast<PreferencesWidgets*>(data);

            GtkWidget* dialog = gtk_file_chooser_dialog_new(
                "Seleccionar fondo de pantalla", nullptr, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancelar",
                GTK_RESPONSE_CANCEL, "Seleccionar", GTK_RESPONSE_ACCEPT, nullptr);

            GtkFileFilter* filter = gtk_file_filter_new();

            gtk_file_filter_set_name(filter, "Imágenes");

            gtk_file_filter_add_mime_type(filter, "image/png");
            gtk_file_filter_add_mime_type(filter, "image/jpeg");
            gtk_file_filter_add_mime_type(filter, "image/webp");

            gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

            if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
                gchar* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

                if (filename != nullptr) {
                    gtk_entry_set_text(GTK_ENTRY(widgets->background_image), filename);

                    g_free(filename);
                }
            }

            gtk_widget_destroy(dialog);
        }),
        p);

    gtk_box_pack_start(GTK_BOX(root), appearance, FALSE, FALSE, 0);

    // Privacy
    GtkWidget* privacy_grid = nullptr;

    GtkWidget* privacy = create_section("Privacidad y seguridad", &privacy_grid);

    p->tracking_protection = gtk_check_button_new_with_label("Protección contra rastreo");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->tracking_protection),
                                 get_boolean(key_file, "privacy", "tracking_protection", true));

    gtk_grid_attach(GTK_GRID(privacy_grid), p->tracking_protection, 0, 0, 2, 1);

    p->clear_cookies = gtk_check_button_new_with_label("Eliminar cookies al cerrar el navegador");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->clear_cookies),
                                 get_boolean(key_file, "privacy", "clear_cookies", false));

    gtk_grid_attach(GTK_GRID(privacy_grid), p->clear_cookies, 0, 1, 2, 1);

    gtk_box_pack_start(GTK_BOX(root), privacy, FALSE, FALSE, 0);

    // Bacus Vault
    GtkWidget* vault_grid = nullptr;

    GtkWidget* vault = create_section("Bacus Vault", &vault_grid);

    p->vault_enabled = gtk_check_button_new_with_label("Habilitar Bacus Vault");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->vault_enabled),
                                 get_boolean(key_file, "vault", "enabled", false));

    gtk_grid_attach(GTK_GRID(vault_grid), p->vault_enabled, 0, 0, 2, 1);

    p->vault_auto_lock = gtk_check_button_new_with_label("Bloqueo automático");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->vault_auto_lock),
                                 get_boolean(key_file, "vault", "automatic_lock", true));

    gtk_grid_attach(GTK_GRID(vault_grid), p->vault_auto_lock, 0, 1, 2, 1);

    GtkWidget* vault_info =
        gtk_label_new("El almacenamiento cifrado y la administración "
                      "de contraseñas se implementarán en una etapa posterior.");

    gtk_label_set_line_wrap(GTK_LABEL(vault_info), TRUE);

    gtk_grid_attach(GTK_GRID(vault_grid), vault_info, 0, 2, 2, 1);

    gtk_box_pack_start(GTK_BOX(root), vault, FALSE, FALSE, 0);

    // Extensions
    GtkWidget* extensions_grid = nullptr;

    GtkWidget* extensions = create_section("Extensiones", &extensions_grid);

    p->extensions_enabled = gtk_check_button_new_with_label("Habilitar sistema de extensiones");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->extensions_enabled),
                                 get_boolean(key_file, "extensions", "enabled", true));

    gtk_grid_attach(GTK_GRID(extensions_grid), p->extensions_enabled, 0, 0, 2, 1);

    p->extensions_developer_mode = gtk_check_button_new_with_label("Activar modo desarrollador");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->extensions_developer_mode),
                                 get_boolean(key_file, "extensions", "developer_mode", false));

    gtk_grid_attach(GTK_GRID(extensions_grid), p->extensions_developer_mode, 0, 1, 2, 1);

    GtkWidget* extensions_info = gtk_label_new("El administrador e instalador de extensiones "
                                               "se implementarán próximamente.");

    gtk_label_set_line_wrap(GTK_LABEL(extensions_info), TRUE);

    gtk_grid_attach(GTK_GRID(extensions_grid), extensions_info, 0, 2, 2, 1);

    gtk_box_pack_start(GTK_BOX(root), extensions, FALSE, FALSE, 0);

    // Clima G
    GtkWidget* clima_grid = nullptr;

    GtkWidget* clima = create_section("Clima G", &clima_grid);

    p->clima_enabled = gtk_check_button_new_with_label("Mostrar Clima G en about:home");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->clima_enabled),
                                 get_boolean(key_file, "clima_g", "enabled", true));

    gtk_grid_attach(GTK_GRID(clima_grid), p->clima_enabled, 0, 0, 2, 1);

    p->clima_auto_location = gtk_check_button_new_with_label("Solicitar ubicación automática");

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(p->clima_auto_location),
                                 get_boolean(key_file, "clima_g", "automatic_location", true));

    gtk_grid_attach(GTK_GRID(clima_grid), p->clima_auto_location, 0, 1, 2, 1);

    p->clima_url = gtk_entry_new();

    gtk_entry_set_text(GTK_ENTRY(p->clima_url),
                       bacus::app::get_preference(key_file, "clima_g", "url", DEFAULT_CLIMA_URL).c_str());

    add_setting(clima_grid, 2, "Dirección de Clima G", p->clima_url);

    gtk_box_pack_start(GTK_BOX(root), clima, FALSE, FALSE, 0);

    // Downloads
    GtkWidget* downloads_grid = nullptr;

    GtkWidget* downloads = create_section("Descargas", &downloads_grid);

    p->download_directory = gtk_entry_new();

    const char* default_downloads = g_get_user_special_dir(G_USER_DIRECTORY_DOWNLOAD);

    gtk_entry_set_text(
        GTK_ENTRY(p->download_directory),
        bacus::app::get_preference(key_file, "downloads", "directory",
                   default_downloads != nullptr ? default_downloads : g_get_home_dir())
            .c_str());

    add_setting(downloads_grid, 0, "Carpeta de descargas", p->download_directory);

    gtk_box_pack_start(GTK_BOX(root), downloads, FALSE, FALSE, 0);

    // Performance
    GtkWidget* performance_grid = nullptr;

    GtkWidget* performance = create_section("Rendimiento", &performance_grid);

    p->hardware_acceleration = gtk_check_button_new_with_label("Aceleración por hardware");

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->hardware_acceleration),
        get_boolean(key_file, "performance", "hardware_acceleration", true));

    gtk_grid_attach(GTK_GRID(performance_grid), p->hardware_acceleration, 0, 0, 2, 1);

    gtk_box_pack_start(GTK_BOX(root), performance, FALSE, FALSE, 0);

    // Actions
    GtkWidget* actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);

    GtkWidget* save = gtk_button_new_with_label("Guardar configuración");

    GtkWidget* reset = gtk_button_new_with_label("Restaurar valores predeterminados");

    gtk_box_pack_start(GTK_BOX(actions), save, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(actions), reset, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(root), actions, FALSE, FALSE, 0);

    p->status = gtk_label_new("Los cambios se guardan en este dispositivo.");

    gtk_widget_set_halign(p->status, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(root), p->status, FALSE, FALSE, 0);

    g_signal_connect(save, "clicked", G_CALLBACK(save_preferences), p);

    g_signal_connect(reset, "clicked", G_CALLBACK(reset_preferences), p);

    g_object_set_data_full(G_OBJECT(root), "bacus-preferences-widgets", p, g_free);

    g_key_file_unref(key_file);

    return scrolled;
}

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

    // Aplicar fondo local, si existe.
    GKeyFile* key_file = bacus::app::load_preferences();

    const std::string background = bacus::app::get_preference(key_file, "appearance", "background_image", "");

    g_key_file_unref(key_file);

    if (!background.empty() && g_file_test(background.c_str(), G_FILE_TEST_IS_REGULAR)) {
        gchar* uri = g_filename_to_uri(background.c_str(), nullptr, nullptr);

        if (uri != nullptr) {
            gchar* css = g_strdup_printf("box { background-image: url(\"%s\"); "
                                         "background-size: cover; "
                                         "background-position: center; }",
                                         uri);

            GtkCssProvider* provider = gtk_css_provider_new();

            gtk_css_provider_load_from_data(provider, css, -1, nullptr);

            gtk_style_context_add_provider(gtk_widget_get_style_context(root),
                                           GTK_STYLE_PROVIDER(provider),
                                           GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

            g_object_unref(provider);

            g_free(css);
            g_free(uri);
        }
    }

    GtkWidget* title = gtk_label_new(nullptr);

    gtk_label_set_markup(GTK_LABEL(title), "<span size='xx-large' weight='bold'>"
                                           "Bacus Explorer"
                                           "</span>");

    gtk_box_pack_start(GTK_BOX(root), title, FALSE, FALSE, 0);

    GtkWidget* subtitle = gtk_label_new("Tu navegador, tu espacio.");

    gtk_box_pack_start(GTK_BOX(root), subtitle, FALSE, FALSE, 0);

    GtkWidget* welcome = gtk_label_new("Bienvenido a Bacus Explorer 0.1.0");

    gtk_box_pack_start(GTK_BOX(root), welcome, FALSE, FALSE, 12);

    GtkWidget* description = gtk_label_new("Escribe una dirección web en la barra superior "
                                           "para comenzar.\n\n"
                                           "El motor Chromium estará disponible en una "
                                           "próxima etapa.");

    gtk_label_set_justify(GTK_LABEL(description), GTK_JUSTIFY_CENTER);

    gtk_label_set_line_wrap(GTK_LABEL(description), TRUE);

    gtk_box_pack_start(GTK_BOX(root), description, FALSE, FALSE, 0);

    GtkWidget* company = gtk_label_new(bacus::core::COMPANY.data());

    gtk_box_pack_start(GTK_BOX(root), company, FALSE, FALSE, 12);

    return root;
}

GtkWidget* create_blank_page() {
    GtkWidget* page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    gtk_widget_set_hexpand(page, TRUE);
    gtk_widget_set_vexpand(page, TRUE);

    return page;
}

GtkWidget* create_error_page(const std::string& uri) {
    GtkWidget* root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);

    gtk_widget_set_halign(root, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(root, GTK_ALIGN_CENTER);

    GtkWidget* title = gtk_label_new("Página interna no disponible");

    gtk_box_pack_start(GTK_BOX(root), title, FALSE, FALSE, 0);

    GtkWidget* address = gtk_label_new(uri.c_str());

    gtk_box_pack_start(GTK_BOX(root), address, FALSE, FALSE, 0);

    GtkWidget* description = gtk_label_new("Esta página todavía no está implementada.");

    gtk_box_pack_start(GTK_BOX(root), description, FALSE, FALSE, 0);

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

    if (uri == "about:config" || uri == "about:settings") {
        return create_config_page();
    }

    if (uri.rfind("about:", 0) == 0) {
        return create_error_page(uri);
    }

    return create_error_page(uri);
}

} // namespace bacus::app
