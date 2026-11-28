#include "browser_window.hpp"
#include "command_line.hpp"

#include <gtk/gtk.h>

#include <iostream>
#include <memory>
#include <exception>
#include <string>

namespace {

void on_application_activate(GtkApplication* application, gpointer) {
    if (g_object_get_data(
            G_OBJECT(application),
            "bacus-browser-window") != nullptr) {
        return;
    }

    try {
        const bool open_homepage =
            GPOINTER_TO_INT(g_object_get_data(
                G_OBJECT(application), "bacus-open-homepage")) != 0;

        auto browser =
            std::make_unique<bacus::app::BrowserWindow>(
                application, open_homepage
            );

        browser->show();

        g_object_set_data_full(
            G_OBJECT(application),
            "bacus-browser-window",
            browser.release(),
            [](gpointer data) {
                delete static_cast<bacus::app::BrowserWindow*>(data);
            }
        );

    } catch (const std::exception& error) {
        std::cerr << "Bacus Explorer startup error: "
                  << error.what() << '\n';
    }
}

} // namespace

int main(int argc, char** argv) {
    const auto command_line =
        bacus::app::parse_command_line(argc, argv);

    if (command_line.show_help) {
        std::cout
            << "Bacus Explorer - Navegador web\n\n"
            << "Uso: bacus-explorer [opciones]\n\n"
            << "Opciones disponibles:\n"
            << "  --bacus-ayuda       Muestra esta ayuda\n"
            << "  --bacus-version     Muestra la versión\n"
            << "  --bacus-inicio      Abre la página de inicio\n";

        return 0;
    }

    if (command_line.show_version) {
        std::cout << "Bacus Explorer "
                  << BACUS_EXPLORER_VERSION << '\n';
        return 0;
    }

    try {
        GtkApplication* application = gtk_application_new(
            "com.guafguaf.bacus-explorer",
            G_APPLICATION_DEFAULT_FLAGS
        );

        if (application == nullptr) {
            std::cerr << "No se pudo crear la aplicación GTK.\n";
            return 1;
        }

        g_object_set_data(
            G_OBJECT(application),
            "bacus-open-homepage",
            GINT_TO_POINTER(command_line.open_homepage ? 1 : 0)
        );

        g_signal_connect(
            application,
            "activate",
            G_CALLBACK(on_application_activate),
            nullptr
        );

        std::string program = command_line.program.empty()
            ? "bacus-explorer"
            : command_line.program;

        char* gtk_argv[] = {
            program.data(),
            nullptr
        };

        const int status = g_application_run(
            G_APPLICATION(application),
            1,
            gtk_argv
        );

        g_object_unref(application);
        return status;

    } catch (const std::exception& error) {
        std::cerr << "Bacus Explorer startup error: "
                  << error.what() << '\n';
        return 1;
    }
}
