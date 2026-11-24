#pragma once

#include <gtk/gtk.h>

#include <string>

namespace bacus::app {

GtkWidget* create_internal_page(const std::string& uri);

} // namespace bacus::app
