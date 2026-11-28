#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "../app/command_line.hpp"

namespace {

int failures = 0;

void check(bool condition, const std::string& description) {
    if (condition) {
        std::cout << "[OK] " << description << '\n';
    } else {
        std::cerr << "[ERROR] " << description << '\n';
        ++failures;
    }
}

bacus::app::CommandLine parse(
    const std::vector<std::string>& arguments
) {
    std::vector<char*> argv;

    for (const auto& argument : arguments) {
        argv.push_back(const_cast<char*>(argument.c_str()));
    }

    return bacus::app::parse_command_line(
        static_cast<int>(argv.size()),
        argv.data()
    );
}

} // namespace

int main() {
    std::cout << "Bacus Explorer - Command Line Tests\n\n";

    {
        const auto result = parse({
            "bacus-explorer",
            "--bacus-inicio",
            "--bacus-configuracion",
            "--disable-gpu",
            "--gtk-debug"
        });

        check(result.program == "bacus-explorer",
              "Conserva el nombre del programa");

        check(result.application_arguments.size() == 2,
              "Clasifica los argumentos propios de Bacus");

        check(result.application_arguments[0] == "--bacus-inicio",
              "Reconoce --bacus-inicio");

        check(result.application_arguments[1] == "--bacus-configuracion",
              "Reconoce --bacus-configuracion");

        check(result.chromium_arguments.size() == 1 &&
              result.chromium_arguments[0] == "--disable-gpu",
              "Clasifica argumentos de Chromium");

        check(result.gtk_arguments.size() == 1 &&
              result.gtk_arguments[0] == "--gtk-debug",
              "Clasifica argumentos de GTK");

        check(result.original_arguments.size() == 4,
              "Conserva los argumentos originales");
    }

    {
        const auto result = parse({
            "bacus-explorer",
            "--",
            "--bacus-inicio",
            "--gtk-debug"
        });

        check(result.application_arguments.size() == 1 &&
              result.application_arguments[0] == "--",
              "Reconoce el separador de argumentos");

        check(result.chromium_arguments.size() == 2,
              "Después de -- envía los argumentos a Chromium");
    }

    {
        const auto result = parse({
            "bacus-explorer",
            "--bacus-ayuda",
            "--bacus-version"
        });

        check(result.application_arguments.size() == 2,
              "Reconoce ayuda y versión en español");

        check(result.show_help,
              "Activa el indicador de ayuda");

        check(result.show_version,
              "Activa el indicador de versión");

        check(!result.open_homepage,
              "No activa el inicio cuando no se solicita");
    }

    {
        char* argv[] = {nullptr};
        const auto result =
            bacus::app::parse_command_line(0, argv);

        check(result.program.empty(),
              "Acepta una entrada vacía");
    }

    {
        const auto result =
            bacus::app::parse_command_line(1, nullptr);

        check(result.program.empty(),
              "Acepta un puntero argv nulo");
    }

    std::cout << "\n";

    if (failures != 0) {
        std::cerr << failures << " prueba(s) fallaron.\n";
        return EXIT_FAILURE;
    }

    std::cout << "Todas las pruebas pasaron.\n";
    return EXIT_SUCCESS;
}
