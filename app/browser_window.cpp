#include "browser_window.hpp"

#include "bacus-core/browser_config.hpp"
#include "internal_pages.hpp"
#include "preferences.hpp"

#include <webkit2/webkit2.h>
#include <gtk/gtk.h>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace bacus::app {

namespace {

std::string get_search_engine() {
    GKeyFile* key_file = load_preferences();

    std::string engine = get_preference(
        key_file, "search", "engine", "Google"
    );

    g_key_file_unref(key_file);

    if (engine != "Google" &&
        engine != "DuckDuckGo" &&
        engine != "Bing" &&
        engine != "Wikipedia") {
        engine = "Google";
    }

    return engine;
}

std::string get_homepage() {
    GKeyFile* key_file = load_preferences();

    std::string homepage = get_preference(
        key_file, "general", "homepage", "about:home"
    );

    g_key_file_unref(key_file);

    if (homepage.empty()) {
        homepage = "about:home";
    }

    return homepage;
}

std::string make_search_url(const std::string& query) {
    gchar* encoded = g_uri_escape_string(
        query.c_str(), nullptr, FALSE);

    if (!encoded) {
        return "https://www.google.com/";
    }

    const std::string engine = get_search_engine();
    std::string url;

    if (engine == "DuckDuckGo") {
        url = "https://duckduckgo.com/?q=";
    } else if (engine == "Bing") {
        url = "https://www.bing.com/search?q=";
    } else if (engine == "Wikipedia") {
        url = "https://es.wikipedia.org/w/index.php?search=";
    } else {
        url = "https://www.google.com/search?q=";
    }

    url += encoded;
    g_free(encoded);

    return url;
}

std::string normalize_address(const std::string& input) {
    const auto first = input.find_first_not_of(" \t\n\r");

    if (first == std::string::npos) {
        return "about:home";
    }

    const auto last = input.find_last_not_of(" \\t\\n\\r");
    const std::string value = input.substr(first, last - first + 1);

    // Páginas internas de Bacus.
    if (value.rfind("about:", 0) == 0) {
        return value;
    }

    // Direcciones locales: reconocerlas antes de interpretar los dos puntos.
    if (value == "localhost" ||
        value.rfind("localhost:", 0) == 0 ||
        value.rfind("localhost/", 0) == 0) {
        return "http://" + value;
    }

    // Las URL con esquema explícito se conservan.
    if (value.find("://") != std::string::npos ||
        value.rfind("mailto:", 0) == 0 ||
        value.rfind("file:", 0) == 0 ||
        value.rfind("data:", 0) == 0) {
        return value;
    }

    // Una frase o texto con espacios se considera una búsqueda.
    if (value.find_first_of(" \t\n") != std::string::npos) {
        return make_search_url(value);
    }

    // Cualquier dirección con puntos se considera un dominio o una IP.
    // Esto también admite puertos y rutas, por ejemplo:
    // climag.duckdns.org:8080/mapa
    if (value.find('.') != std::string::npos) {
        return "https://" + value;
    }

    // Los nombres simples sin puntos se buscan.
    return make_search_url(value);
}

} // namespace

struct BrowserWindow::Impl {

    struct TabState {
        GtkWidget* page = nullptr;
        GtkWidget* stack = nullptr;
        GtkWidget* address_label = nullptr;
        GtkWidget* web_view_widget = nullptr;
        GtkWidget* internal_page = nullptr;
        GtkWidget* tab_label = nullptr;
        GtkWidget* tab_title_label = nullptr;

        WebKitWebView* web_view = nullptr;
        Impl* owner = nullptr;

        std::string address = "about:newtab";
    };

    GtkWidget* window = nullptr;
    GtkWidget* notebook = nullptr;
    GtkWidget* address_entry = nullptr;

    GtkWidget* back_button = nullptr;
    GtkWidget* forward_button = nullptr;
    GtkWidget* reload_button = nullptr;
    GtkWidget* status_label = nullptr;

    std::vector<std::shared_ptr<TabState>> tabs;

    explicit Impl(GtkApplication* application, bool open_homepage) {
        window = gtk_application_window_new(application);

        gtk_window_set_title(
            GTK_WINDOW(window), "Bacus Explorer");

        gtk_window_set_default_size(
            GTK_WINDOW(window),
            bacus::core::DEFAULT_WINDOW_WIDTH,
            bacus::core::DEFAULT_WINDOW_HEIGHT
        );

        GtkWidget* root = gtk_box_new(
            GTK_ORIENTATION_VERTICAL, 0);

        gtk_container_add(GTK_CONTAINER(window), root);

        GtkWidget* toolbar = gtk_box_new(
            GTK_ORIENTATION_HORIZONTAL, 5);

        gtk_container_set_border_width(
            GTK_CONTAINER(toolbar), 6);

        gtk_box_pack_start(
            GTK_BOX(root), toolbar, FALSE, FALSE, 0);

        back_button = gtk_button_new_with_label("←");
        forward_button = gtk_button_new_with_label("→");
        reload_button = gtk_button_new_with_label("↻");

        gtk_widget_set_tooltip_text(back_button, "Atrás");
        gtk_widget_set_tooltip_text(forward_button, "Adelante");
        gtk_widget_set_tooltip_text(reload_button, "Recargar");

        gtk_box_pack_start(
            GTK_BOX(toolbar), back_button, FALSE, FALSE, 0);

        gtk_box_pack_start(
            GTK_BOX(toolbar), forward_button, FALSE, FALSE, 0);

        gtk_box_pack_start(
            GTK_BOX(toolbar), reload_button, FALSE, FALSE, 0);

        address_entry = gtk_entry_new();

        gtk_entry_set_placeholder_text(
            GTK_ENTRY(address_entry),
            "Buscar o introducir una dirección"
        );

        gtk_widget_set_hexpand(address_entry, TRUE);

        gtk_box_pack_start(
            GTK_BOX(toolbar), address_entry, TRUE, TRUE, 0);

        GtkWidget* navigate_button =
            gtk_button_new_with_label("Ir");

        gtk_box_pack_start(
            GTK_BOX(toolbar), navigate_button, FALSE, FALSE, 0);

        GtkWidget* new_tab_button =
            gtk_button_new_with_label("+");

        gtk_widget_set_tooltip_text(
            new_tab_button, "Nueva pestaña");

        gtk_box_pack_start(
            GTK_BOX(toolbar), new_tab_button, FALSE, FALSE, 0);

        notebook = gtk_notebook_new();

        gtk_notebook_set_scrollable(
            GTK_NOTEBOOK(notebook), TRUE);

        gtk_box_pack_start(
            GTK_BOX(root), notebook, TRUE, TRUE, 0);

        status_label = gtk_label_new(
            "Bacus Explorer 0.1.0 | WebKitGTK");

        gtk_widget_set_halign(
            status_label, GTK_ALIGN_START);

        gtk_box_pack_start(
            GTK_BOX(root), status_label, FALSE, FALSE, 5);

        g_signal_connect(
            navigate_button, "clicked",
            G_CALLBACK(+[](GtkButton*, gpointer data) {
                static_cast<Impl*>(data)->navigate();
            }), this);

        g_signal_connect(
            address_entry, "activate",
            G_CALLBACK(+[](GtkEntry*, gpointer data) {
                static_cast<Impl*>(data)->navigate();
            }), this);

        g_signal_connect(
            new_tab_button, "clicked",
            G_CALLBACK(+[](GtkButton*, gpointer data) {
                static_cast<Impl*>(data)->add_tab();
            }), this);

        g_signal_connect(
            back_button, "clicked",
            G_CALLBACK(+[](GtkButton*, gpointer data) {
                auto* self = static_cast<Impl*>(data);
                auto tab = self->current_tab();

                if (tab && webkit_web_view_can_go_back(tab->web_view)) {
                    webkit_web_view_go_back(tab->web_view);
                }
            }), this);

        g_signal_connect(
            forward_button, "clicked",
            G_CALLBACK(+[](GtkButton*, gpointer data) {
                auto* self = static_cast<Impl*>(data);
                auto tab = self->current_tab();

                if (tab && webkit_web_view_can_go_forward(tab->web_view)) {
                    webkit_web_view_go_forward(tab->web_view);
                }
            }), this);

        g_signal_connect(
            reload_button, "clicked",
            G_CALLBACK(+[](GtkButton*, gpointer data) {
                auto* self = static_cast<Impl*>(data);
                auto tab = self->current_tab();

                if (tab) {
                    webkit_web_view_reload(tab->web_view);
                }
            }), this);

        g_signal_connect(
            notebook, "switch-page",
            G_CALLBACK(+[](GtkNotebook*, GtkWidget*, guint, gpointer data) {
                auto* self = static_cast<Impl*>(data);
                self->update_address();
                self->update_navigation_buttons();
            }), this);

        add_tab(open_homepage ? get_homepage() : "about:newtab");
    }

    std::shared_ptr<TabState> current_tab() {
        const int index = gtk_notebook_get_current_page(
            GTK_NOTEBOOK(notebook));

        if (index < 0 ||
            static_cast<std::size_t>(index) >= tabs.size()) {
            return nullptr;
        }

        return tabs[static_cast<std::size_t>(index)];
    }

    void set_status(const std::string& message) {
        gtk_label_set_text(
            GTK_LABEL(status_label), message.c_str());
    }

    void update_address() {
        auto tab = current_tab();

        if (!tab) {
            return;
        }

        gtk_entry_set_text(
            GTK_ENTRY(address_entry), tab->address.c_str());
    }

    void update_navigation_buttons() {
        auto tab = current_tab();

        gtk_widget_set_sensitive(
            back_button,
            tab && webkit_web_view_can_go_back(tab->web_view));

        gtk_widget_set_sensitive(
            forward_button,
            tab && webkit_web_view_can_go_forward(tab->web_view));
    }

    void update_tab_title(
        const std::shared_ptr<TabState>& tab) {

        const gchar* title =
            webkit_web_view_get_title(tab->web_view);

        if (title && *title) {
            gtk_label_set_text(
                GTK_LABEL(tab->tab_title_label), title);
        } else {
            gtk_label_set_text(
                GTK_LABEL(tab->tab_title_label), "Nueva pestaña");
        }
    }

    void show_internal_page(
        const std::shared_ptr<TabState>& tab,
        const std::string& uri) {

        if (tab->internal_page) {
            gtk_container_remove(
                GTK_CONTAINER(tab->stack),
                tab->internal_page);

            tab->internal_page = nullptr;
        }

        tab->internal_page = create_internal_page(uri);

        if (!tab->internal_page) {
            tab->internal_page = gtk_label_new(
                "No se pudo crear la página interna.");
        }

        gtk_stack_add_named(
            GTK_STACK(tab->stack),
            tab->internal_page,
            "internal"
        );

        gtk_widget_show_all(tab->internal_page);

        gtk_stack_set_visible_child_name(
            GTK_STACK(tab->stack), "internal");

        tab->address = uri;

        update_tab_title(tab);
        update_address();
        update_navigation_buttons();

        set_status("Página interna de Bacus Explorer");
    }

    void navigate_to(
        const std::shared_ptr<TabState>& tab,
        const std::string& destination) {

        if (!tab) {
            return;
        }

        const std::string uri =
            normalize_address(destination);

        tab->address = uri;

        if (uri.rfind("about:", 0) == 0) {
            webkit_web_view_stop_loading(tab->web_view);
            show_internal_page(tab, uri);
            return;
        }

        if (tab->internal_page) {
            gtk_container_remove(
                GTK_CONTAINER(tab->stack),
                tab->internal_page);

            tab->internal_page = nullptr;
        }

        gtk_stack_set_visible_child_name(
            GTK_STACK(tab->stack), "web");

        update_address();
        set_status("Cargando página...");

        webkit_web_view_load_uri(
            tab->web_view, uri.c_str());
    }

    void navigate() {
        auto tab = current_tab();

        if (!tab) {
            return;
        }

        const gchar* text =
            gtk_entry_get_text(GTK_ENTRY(address_entry));

        navigate_to(tab, text ? text : "");
    }

    void close_tab(TabState* target) {
        if (!target) {
            return;
        }

        auto it = tabs.begin();

        while (it != tabs.end() && it->get() != target) {
            ++it;
        }

        if (it == tabs.end()) {
            return;
        }

        auto tab = *it;
        const int page_index = gtk_notebook_page_num(
            GTK_NOTEBOOK(notebook), tab->page);

        if (page_index < 0) {
            return;
        }

        webkit_web_view_stop_loading(tab->web_view);

        tabs.erase(it);

        gtk_notebook_remove_page(
            GTK_NOTEBOOK(notebook), page_index);

        gtk_widget_destroy(tab->page);

        if (tabs.empty()) {
            add_tab();
            return;
        }

        update_address();
        update_navigation_buttons();
        set_status("Pestaña cerrada");
    }

    void add_tab(const std::string& initial_uri = "about:newtab") {
        auto tab = std::make_shared<TabState>();
        tab->owner = this;

        tab->page = gtk_box_new(
            GTK_ORIENTATION_VERTICAL, 0);

        tab->stack = gtk_stack_new();

        gtk_widget_set_hexpand(tab->stack, TRUE);
        gtk_widget_set_vexpand(tab->stack, TRUE);

        gtk_box_pack_start(
            GTK_BOX(tab->page),
            tab->stack, TRUE, TRUE, 0);

        tab->web_view_widget = webkit_web_view_new();

        tab->web_view = WEBKIT_WEB_VIEW(
            tab->web_view_widget);

        // Identidad propia de Bacus Explorer, conservando el User-Agent de WebKit.
        WebKitSettings* settings =
            webkit_web_view_get_settings(tab->web_view);

        webkit_settings_set_user_agent_with_application_details(
            settings,
            "BacusExplorer",
            BACUS_EXPLORER_VERSION
        );

        gtk_stack_add_named(
            GTK_STACK(tab->stack),
            tab->web_view_widget,
            "web");

        g_signal_connect(
            tab->web_view, "notify::uri",
            G_CALLBACK(+[](GObject* object, GParamSpec*, gpointer data) {
                auto* tab = static_cast<TabState*>(data);
                const gchar* uri = webkit_web_view_get_uri(
                    WEBKIT_WEB_VIEW(object));

                if (uri && tab->address.rfind("about:", 0) != 0) {
                    tab->address = uri;
                    tab->owner->update_address();
                }
            }), tab.get());

        g_signal_connect(
            tab->web_view, "notify::title",
            G_CALLBACK(+[](GObject*, GParamSpec*, gpointer data) {
                auto* tab = static_cast<TabState*>(data);
                tab->owner->update_tab_title(
                    tab->owner->current_tab());
            }), tab.get());

        g_signal_connect(
            tab->web_view, "load-changed",
            G_CALLBACK(+[](WebKitWebView* view,
                           WebKitLoadEvent event,
                           gpointer data) {
                auto* tab = static_cast<TabState*>(data);

                if (event == WEBKIT_LOAD_STARTED) {
                    tab->owner->set_status("Cargando...");
                } else if (event == WEBKIT_LOAD_FINISHED) {
                    const gchar* uri =
                        webkit_web_view_get_uri(view);

                    if (uri) {
                        tab->address = uri;
                    }

                    tab->owner->update_address();
                    tab->owner->update_tab_title(
                        tab->owner->current_tab());

                    tab->owner->update_navigation_buttons();
                    tab->owner->set_status("Página cargada");
                }
            }), tab.get());

        g_signal_connect(
            tab->web_view, "load-failed",
            G_CALLBACK(+[](WebKitWebView*, WebKitLoadEvent,
                           const gchar*, GError* error,
                           gpointer data) -> gboolean {
                auto* tab = static_cast<TabState*>(data);

                if (error) {
                    tab->owner->set_status(
                        std::string("Error de carga: ") +
                        error->message);
                }

                return FALSE;
            }), tab.get());

        tab->tab_label = gtk_box_new(
            GTK_ORIENTATION_HORIZONTAL, 5);

        tab->tab_title_label = gtk_label_new("Nueva pestaña");

        gtk_box_pack_start(
            GTK_BOX(tab->tab_label),
            tab->tab_title_label,
            TRUE, TRUE, 0);

        GtkWidget* close_button = gtk_button_new_with_label("×");

        gtk_widget_set_tooltip_text(
            close_button, "Cerrar esta pestaña");

        gtk_button_set_relief(
            GTK_BUTTON(close_button), GTK_RELIEF_NONE);

        gtk_widget_set_focus_on_click(close_button, FALSE);

        gtk_box_pack_start(
            GTK_BOX(tab->tab_label),
            close_button,
            FALSE, FALSE, 0);

        g_signal_connect(
            close_button, "clicked",
            G_CALLBACK(+[](GtkButton*, gpointer data) {
                auto* tab = static_cast<TabState*>(data);
                tab->owner->close_tab(tab);
            }), tab.get());

        const int index = gtk_notebook_append_page(
            GTK_NOTEBOOK(notebook),
            tab->page,
            tab->tab_label
        );

        gtk_widget_show_all(tab->tab_label);

        tabs.push_back(tab);

        gtk_widget_show_all(tab->page);

        gtk_notebook_set_current_page(
            GTK_NOTEBOOK(notebook), index);

        navigate_to(tab, initial_uri);
    }
};

BrowserWindow::BrowserWindow(GtkApplication* application, bool open_homepage)
    : impl_(std::make_unique<Impl>(application, open_homepage)) {}

BrowserWindow::~BrowserWindow() = default;

void BrowserWindow::show() {
    gtk_widget_show_all(impl_->window);
}

} // namespace bacus::app
