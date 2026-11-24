
#include "browser_window.hpp"

#include <gtk/gtk.h>

#include <exception>
#include <iostream>

namespace {

void on_application_activate(GtkApplication* application, gpointer) {
    try {
        auto* browser = new bacus::app::BrowserWindow(application);

        g_object_set_data_full(
            G_OBJECT(application), "bacus-browser-window", browser,
            [](gpointer data) { delete static_cast<bacus::app::BrowserWindow*>(data); });

        browser->show();

    } catch (const std::exception& error) {
        std::cerr << "Bacus Explorer startup error: " << error.what() << '\n';
    }
}

} // namespace

int main(int argc, char** argv) {
    GtkApplication* application =
        gtk_application_new("com.guafguaf.bacus-explorer", G_APPLICATION_FLAGS_NONE);

    g_signal_connect(application, "activate", G_CALLBACK(on_application_activate), nullptr);

    const int status = g_application_run(G_APPLICATION(application), argc, argv);

    g_object_unref(application);

    return status;
}
