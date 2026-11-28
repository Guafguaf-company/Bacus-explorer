#pragma once

#include <string>
#include <vector>

namespace bacus::app {

struct CommandLine {
    std::string program;

    // Línea original, conservada sin modificaciones.
    std::vector<std::string> original_arguments;

    // Opciones y argumentos propios de Bacus Explorer.
    std::vector<std::string> application_arguments;

    // Opciones y argumentos de Chromium/CEF.
    std::vector<std::string> chromium_arguments;

    // Opciones explícitamente destinadas a GTK.
    std::vector<std::string> gtk_arguments;

    // Argumentos que todavía no tienen clasificación.
    bool show_help = false;
    bool show_version = false;
    bool open_homepage = false;

    std::vector<std::string> unknown_arguments;
};

// Captura y clasifica los argumentos.
CommandLine parse_command_line(int argc, char* argv[]);

} // namespace bacus::app
