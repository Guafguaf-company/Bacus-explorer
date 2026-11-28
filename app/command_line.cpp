#include "command_line.hpp"

namespace bacus::app {

namespace {

bool starts_with(const std::string& value, const std::string& prefix) {
    return value.compare(0, prefix.size(), prefix) == 0;
}

} // namespace

CommandLine parse_command_line(int argc, char* argv[]) {
    CommandLine result;

    if (argc <= 0 || argv == nullptr) {
        return result;
    }

    if (argv[0] != nullptr) {
        result.program = argv[0];
    }

    bool application_arguments_finished = false;

    for (int i = 1; i < argc; ++i) {
        if (argv[i] == nullptr) {
            continue;
        }

        const std::string argument = argv[i];

        result.original_arguments.push_back(argument);

        if (!application_arguments_finished && argument == "--") {
            application_arguments_finished = true;
            result.application_arguments.push_back(argument);
            continue;
        }

        if (!application_arguments_finished &&
            starts_with(argument, "--bacus-")) {
            result.application_arguments.push_back(argument);

            if (argument == "--bacus-ayuda") {
                result.show_help = true;
            } else if (argument == "--bacus-version") {
                result.show_version = true;
            } else if (argument == "--bacus-inicio") {
                result.open_homepage = true;
            }

            continue;
        }

        if (!application_arguments_finished &&
            starts_with(argument, "--gtk-")) {
            result.gtk_arguments.push_back(argument);
            continue;
        }

        result.chromium_arguments.push_back(argument);
    }

    return result;
}

} // namespace bacus::app
