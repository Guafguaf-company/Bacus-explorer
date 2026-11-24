
#include "internal_pages.hpp"

#include "bacus-core/browser_config.hpp"

#include <gtk/gtk.h>

#include <string>

namespace bacus::app {

namespace {

constexpr const char* CONFIG_DIRECTORY = "bacus-explorer";
constexpr const char* CONFIG_FILENAME = "preferences.ini";
constexpr const char* DEFAULT_CLIMA_URL = "https://climag.duckdns.org";
constexpr const char* DEFAULT_SEARCH =
    "https://duckduckgo.com/?q=";

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
    GtkWidget* status = nullptr;
};

std::string get_config_path() {
    gchar* directory = g_build_filename(
        g_get_user_config_dir(),
        CONFIG_DIRECTORY,
        nullptr
    );

    gchar* path = g_build_filename(
        directory,
        CONFIG_FILENAME,
        nullptr
    );

    std::string result(path);

    g_free(directory);
    g_free(path);

    return result;
}

GKeyFile* load_preferences() {
    GKeyFile* key_file = g_key_file_new();

    const std::string path = get_config_path();

    GError* error = nullptr;

    if (!g_key_file_load_from_file(
            key_file,
            path.c_str(),
            G_KEY_FILE_NONE,
            &error
        )) {
        if (error != nullptr) {
            g_error_free(error);
        }
    }

    return key_file;
}

std::string get_string(
    GKeyFile* key_file,
    const char* group,
    const char* key,
    const char* fallback
) {
    GError* error = nullptr;

    gchar* value = g_key_file_get_string(
        key_file,
        group,
        key,
        &error
    );

    if (error != nullptr) {
        g_error_free(error);
        return fallback;
    }

    std::string result = value != nullptr ? value : fallback;

    g_free(value);

    return result;
}

bool get_boolean(
    GKeyFile* key_file,
    const char* group,
    const char* key,
    bool fallback
) {
    GError* error = nullptr;

    const gboolean value = g_key_file_get_boolean(
        key_file,
        group,
        key,
        &error
    );

    if (error != nullptr) {
        g_error_free(error);
        return fallback;
    }

    return value != FALSE;
}

void set_entry(
    GKeyFile* key_file,
    const char* group,
    const char* key,
    GtkWidget* widget
) {
    const char* value = gtk_entry_get_text(GTK_ENTRY(widget));

    g_key_file_set_string(
        key_file,
        group,
        key,
        value != nullptr ? value : ""
    );
}

void set_check(
    GKeyFile* key_file,
    const char* group,
    const char* key,
    GtkWidget* widget
) {
    g_key_file_set_boolean(
        key_file,
        group,
        key,
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
    );
}

void set_combo(
    GKeyFile* key_file,
    const char* group,
    const char* key,
    GtkWidget* widget
) {
    gchar* value = gtk_combo_box_text_get_active_text(
        GTK_COMBO_BOX_TEXT(widget)
    );

    if (value != nullptr) {
        g_key_file_set_string(key_file, group, key, value);
        g_free(value);
    }
}

void save_preferences(
    GtkButton*,
    gpointer user_data
) {
    auto* preferences =
        static_cast<PreferencesWidgets*>(user_data);

    GKeyFile* key_file = g_key_file_new();

    set_entry(key_file, "general", "homepage",
              preferences->homepage);

    set_combo(key_file, "search", "engine",
              preferences->search_engine);

    set_combo(key_file, "appearance", "theme",
              preferences->theme);

    set_check(key_file, "general", "restore_tabs",
              preferences->restore_tabs);

    set_check(key_file, "privacy", "tracking_protection",
              preferences->tracking_protection);

    set_check(key_file, "privacy", "clear_cookies",
              preferences->clear_cookies);

    set_check(key_file, "clima_g", "enabled",
              preferences->clima_enabled);

    set_check(key_file, "clima_g", "automatic_location",
              preferences->clima_auto_location);

    set_entry(key_file, "clima_g", "url",
              preferences->clima_url);

    set_entry(key_file, "downloads", "directory",
              preferences->download_directory);

    set_check(key_file, "performance", "hardware_acceleration",
              preferences->hardware_acceleration);

    gchar* directory = g_build_filename(
        g_get_user_config_dir(),
        CONFIG_DIRECTORY,
        nullptr
    );

    if (g_mkdir_with_parents(directory, 0700) != 0) {
        gtk_label_set_text(
            GTK_LABEL(preferences->status),
            "No se pudo crear la carpeta de configuración."
        );

        g_free(directory);
        g_key_file_unref(key_file);
        return;
    }

    g_free(directory);

    gsize length = 0;

    gchar* data = g_key_file_to_data(
        key_file,
        &length,
        nullptr
    );

    const std::string path = get_config_path();

    GError* error = nullptr;

    const gboolean saved = g_file_set_contents(
        path.c_str(),
        data,
        static_cast<gssize>(length),
        &error
    );

    if (saved) {
        gtk_label_set_text(
            GTK_LABEL(preferences->status),
            "Configuración guardada correctamente."
        );
    } else {
        gtk_label_set_text(
            GTK_LABEL(preferences->status),
            error != nullptr
                ? error->message
                : "No se pudo guardar la configuración."
        );
    }

    if (error != nullptr) {
        g_error_free(error);
    }

    g_free(data);
    g_key_file_unref(key_file);
}

void reset_preferences(
    GtkButton*,
    gpointer user_data
) {
    auto* p = static_cast<PreferencesWidgets*>(user_data);

    gtk_entry_set_text(
        GTK_ENTRY(p->homepage),
        "about:home"
    );

    gtk_combo_box_set_active(
        GTK_COMBO_BOX(p->search_engine),
        0
    );

    gtk_combo_box_set_active(
        GTK_COMBO_BOX(p->theme),
        0
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->restore_tabs),
        TRUE
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->tracking_protection),
        TRUE
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->clear_cookies),
        FALSE
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->clima_enabled),
        TRUE
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->clima_auto_location),
        TRUE
    );

    gtk_entry_set_text(
        GTK_ENTRY(p->clima_url),
        DEFAULT_CLIMA_URL
    );

    gtk_entry_set_text(
        GTK_ENTRY(p->download_directory),
        g_get_user_special_dir(G_USER_DIRECTORY_DOWNLOAD)
            != nullptr
            ? g_get_user_special_dir(G_USER_DIRECTORY_DOWNLOAD)
            : g_get_home_dir()
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->hardware_acceleration),
        TRUE
    );

    gtk_label_set_text(
        GTK_LABEL(p->status),
        "Valores predeterminados restaurados. Pulsa Guardar para aplicarlos."
    );
}

GtkWidget* create_section(
    const char* title,
    GtkWidget** grid_output
) {
    GtkWidget* frame = gtk_frame_new(title);

    GtkWidget* grid = gtk_grid_new();

    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    gtk_container_set_border_width(GTK_CONTAINER(grid), 12);

    gtk_container_add(GTK_CONTAINER(frame), grid);

    *grid_output = grid;

    return frame;
}

void add_setting(
    GtkWidget* grid,
    int row,
    const char* label,
    GtkWidget* control
) {
    GtkWidget* text = gtk_label_new(label);

    gtk_widget_set_halign(text, GTK_ALIGN_START);
    gtk_widget_set_valign(text, GTK_ALIGN_CENTER);

    gtk_grid_attach(GTK_GRID(grid), text, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), control, 1, row, 1, 1);

    gtk_widget_set_hexpand(control, TRUE);
}

GtkWidget* create_config_page() {
    GKeyFile* key_file = load_preferences();

    auto* p = g_new0(PreferencesWidgets, 1);

    GtkWidget* root = gtk_box_new(
        GTK_ORIENTATION_VERTICAL,
        16
    );

    gtk_container_set_border_width(
        GTK_CONTAINER(root),
        24
    );

    GtkWidget* title = gtk_label_new(nullptr);

    gtk_label_set_markup(
        GTK_LABEL(title),
        "<span size='xx-large' weight='bold'>"
        "Configuración de Bacus Explorer"
        "</span>"
    );

    gtk_widget_set_halign(title, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(root), title, FALSE, FALSE, 0);

    GtkWidget* subtitle = gtk_label_new(
        "Administra todas las preferencias del navegador."
    );

    gtk_widget_set_halign(subtitle, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(root), subtitle, FALSE, FALSE, 0);

    // General
    GtkWidget* general_grid = nullptr;

    GtkWidget* general = create_section(
        "General",
        &general_grid
    );

    p->homepage = gtk_entry_new();

    gtk_entry_set_text(
        GTK_ENTRY(p->homepage),
        get_string(
            key_file, "general", "homepage", "about:home"
        ).c_str()
    );

    add_setting(
        general_grid, 0,
        "Página de inicio",
        p->homepage
    );

    p->restore_tabs = gtk_check_button_new_with_label(
        "Restaurar pestañas anteriores al iniciar"
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->restore_tabs),
        get_boolean(key_file, "general", "restore_tabs", true)
    );

    gtk_grid_attach(
        GTK_GRID(general_grid),
        p->restore_tabs,
        0, 1, 2, 1
    );

    gtk_box_pack_start(GTK_BOX(root), general, FALSE, FALSE, 0);

    // Search
    GtkWidget* search_grid = nullptr;

    GtkWidget* search = create_section(
        "Búsqueda",
        &search_grid
    );

    p->search_engine = gtk_combo_box_text_new();

    gtk_combo_box_text_append_text(
        GTK_COMBO_BOX_TEXT(p->search_engine),
        "DuckDuckGo"
    );

    gtk_combo_box_text_append_text(
        GTK_COMBO_BOX_TEXT(p->search_engine),
        "Google"
    );

    gtk_combo_box_text_append_text(
        GTK_COMBO_BOX_TEXT(p->search_engine),
        "Bing"
    );

    const std::string search_value = get_string(
        key_file, "search", "engine", "DuckDuckGo"
    );

    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(p->search_engine),
        nullptr
    );

    if (search_value == "Google") {
        gtk_combo_box_set_active(GTK_COMBO_BOX(p->search_engine), 1);
    } else if (search_value == "Bing") {
        gtk_combo_box_set_active(GTK_COMBO_BOX(p->search_engine), 2);
    } else {
        gtk_combo_box_set_active(GTK_COMBO_BOX(p->search_engine), 0);
    }

    add_setting(
        search_grid, 0,
        "Buscador predeterminado",
        p->search_engine
    );

    gtk_box_pack_start(GTK_BOX(root), search, FALSE, FALSE, 0);

    // Appearance
    GtkWidget* appearance_grid = nullptr;

    GtkWidget* appearance = create_section(
        "Apariencia",
        &appearance_grid
    );

    p->theme = gtk_combo_box_text_new();

    gtk_combo_box_text_append_text(
        GTK_COMBO_BOX_TEXT(p->theme), "Sistema"
    );

    gtk_combo_box_text_append_text(
        GTK_COMBO_BOX_TEXT(p->theme), "Claro"
    );

    gtk_combo_box_text_append_text(
        GTK_COMBO_BOX_TEXT(p->theme), "Oscuro"
    );

    const std::string theme_value = get_string(
        key_file, "appearance", "theme", "Sistema"
    );

    int theme_index = 0;

    if (theme_value == "Claro") {
        theme_index = 1;
    } else if (theme_value == "Oscuro") {
        theme_index = 2;
    }

    gtk_combo_box_set_active(
        GTK_COMBO_BOX(p->theme),
        theme_index
    );

    add_setting(
        appearance_grid, 0,
        "Tema del navegador",
        p->theme
    );

    gtk_box_pack_start(
        GTK_BOX(root), appearance, FALSE, FALSE, 0
    );

    // Privacy
    GtkWidget* privacy_grid = nullptr;

    GtkWidget* privacy = create_section(
        "Privacidad y seguridad",
        &privacy_grid
    );

    p->tracking_protection = gtk_check_button_new_with_label(
        "Protección contra rastreo"
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->tracking_protection),
        get_boolean(key_file, "privacy", "tracking_protection", true)
    );

    gtk_grid_attach(
        GTK_GRID(privacy_grid),
        p->tracking_protection,
        0, 0, 2, 1
    );

    p->clear_cookies = gtk_check_button_new_with_label(
        "Eliminar cookies al cerrar el navegador"
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->clear_cookies),
        get_boolean(key_file, "privacy", "clear_cookies", false)
    );

    gtk_grid_attach(
        GTK_GRID(privacy_grid),
        p->clear_cookies,
        0, 1, 2, 1
    );

    gtk_box_pack_start(
        GTK_BOX(root), privacy, FALSE, FALSE, 0
    );

    // Clima G
    GtkWidget* clima_grid = nullptr;

    GtkWidget* clima = create_section(
        "Clima G",
        &clima_grid
    );

    p->clima_enabled = gtk_check_button_new_with_label(
        "Mostrar Clima G en about:home"
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->clima_enabled),
        get_boolean(key_file, "clima_g", "enabled", true)
    );

    gtk_grid_attach(
        GTK_GRID(clima_grid),
        p->clima_enabled,
        0, 0, 2, 1
    );

    p->clima_auto_location = gtk_check_button_new_with_label(
        "Solicitar ubicación automática"
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->clima_auto_location),
        get_boolean(key_file, "clima_g", "automatic_location", true)
    );

    gtk_grid_attach(
        GTK_GRID(clima_grid),
        p->clima_auto_location,
        0, 1, 2, 1
    );

    p->clima_url = gtk_entry_new();

    gtk_entry_set_text(
        GTK_ENTRY(p->clima_url),
        get_string(
            key_file, "clima_g", "url", DEFAULT_CLIMA_URL
        ).c_str()
    );

    add_setting(
        clima_grid, 2,
        "Dirección de Clima G",
        p->clima_url
    );

    gtk_box_pack_start(GTK_BOX(root), clima, FALSE, FALSE, 0);

    // Downloads
    GtkWidget* downloads_grid = nullptr;

    GtkWidget* downloads = create_section(
        "Descargas",
        &downloads_grid
    );

    p->download_directory = gtk_entry_new();

    const char* default_downloads =
        g_get_user_special_dir(G_USER_DIRECTORY_DOWNLOAD);

    gtk_entry_set_text(
        GTK_ENTRY(p->download_directory),
        get_string(
            key_file,
            "downloads",
            "directory",
            default_downloads != nullptr
                ? default_downloads
                : g_get_home_dir()
        ).c_str()
    );

    add_setting(
        downloads_grid, 0,
        "Carpeta de descargas",
        p->download_directory
    );

    gtk_box_pack_start(
        GTK_BOX(root), downloads, FALSE, FALSE, 0
    );

    // Performance
    GtkWidget* performance_grid = nullptr;

    GtkWidget* performance = create_section(
        "Rendimiento",
        &performance_grid
    );

    p->hardware_acceleration = gtk_check_button_new_with_label(
        "Aceleración por hardware"
    );

    gtk_toggle_button_set_active(
        GTK_TOGGLE_BUTTON(p->hardware_acceleration),
        get_boolean(
            key_file,
            "performance",
            "hardware_acceleration",
            true
        )
    );

    gtk_grid_attach(
        GTK_GRID(performance_grid),
        p->hardware_acceleration,
        0, 0, 2, 1
    );

    gtk_box_pack_start(
        GTK_BOX(root), performance, FALSE, FALSE, 0
    );

    // Actions
    GtkWidget* actions = gtk_box_new(
        GTK_ORIENTATION_HORIZONTAL,
        8
    );

    GtkWidget* save = gtk_button_new_with_label(
        "Guardar configuración"
    );

    GtkWidget* reset = gtk_button_new_with_label(
        "Restaurar valores predeterminados"
    );

    gtk_box_pack_start(
        GTK_BOX(actions), save, FALSE, FALSE, 0
    );

    gtk_box_pack_start(
        GTK_BOX(actions), reset, FALSE, FALSE, 0
    );

    gtk_box_pack_start(
        GTK_BOX(root), actions, FALSE, FALSE, 0
    );

    p->status = gtk_label_new(
        "Los cambios se guardan en este dispositivo."
    );

    gtk_widget_set_halign(p->status, GTK_ALIGN_START);

    gtk_box_pack_start(
        GTK_BOX(root), p->status, FALSE, FALSE, 0
    );

    g_signal_connect(
        save,
        "clicked",
        G_CALLBACK(save_preferences),
        p
    );

    g_signal_connect(
        reset,
        "clicked",
        G_CALLBACK(reset_preferences),
        p
    );

    g_object_set_data_full(
        G_OBJECT(root),
        "bacus-preferences-widgets",
        p,
        g_free
    );

    g_key_file_unref(key_file);

    return root;
}

GtkWidget* create_home_page() {
    GtkWidget* root = gtk_box_new(
        GTK_ORIENTATION_VERTICAL,
        18
    );

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
        "<span size='xx-large' weight='bold'>"
        "Bacus Explorer"
        "</span>"
    );

    gtk_box_pack_start(GTK_BOX(root), title, FALSE, FALSE, 0);

    GtkWidget* subtitle = gtk_label_new(
        "Tu navegador, tu espacio."
    );

    gtk_box_pack_start(
        GTK_BOX(root), subtitle, FALSE, FALSE, 0
    );

    GtkWidget* welcome = gtk_label_new(
        "Bienvenido a Bacus Explorer 0.1.0"
    );

    gtk_box_pack_start(
        GTK_BOX(root), welcome, FALSE, FALSE, 12
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
        GTK_BOX(root), description, FALSE, FALSE, 0
    );

    GtkWidget* company = gtk_label_new(
        bacus::core::COMPANY.data()
    );

    gtk_box_pack_start(
        GTK_BOX(root), company, FALSE, FALSE, 12
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
        GTK_BOX(root), title, FALSE, FALSE, 0
    );

    GtkWidget* address = gtk_label_new(uri.c_str());

    gtk_box_pack_start(
        GTK_BOX(root), address, FALSE, FALSE, 0
    );

    GtkWidget* description = gtk_label_new(
        "Esta página todavía no está implementada."
    );

    gtk_box_pack_start(
        GTK_BOX(root), description, FALSE, FALSE, 0
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

    if (uri == "about:config" || uri == "about:settings") {
        return create_config_page();
    }

    if (uri.rfind("about:", 0) == 0) {
        return create_error_page(uri);
    }

    return create_error_page(uri);
}

} // namespace bacus::app
