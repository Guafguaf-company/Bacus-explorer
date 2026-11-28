#include "preferences.hpp"

#include <glib/gstdio.h>

namespace bacus::app {

namespace {

constexpr const char* CONFIG_DIRECTORY = "bacus-explorer";
constexpr const char* CONFIG_FILENAME = "preferences.ini";

std::string get_config_directory() {
    gchar* directory = g_build_filename(
        g_get_user_config_dir(),
        CONFIG_DIRECTORY,
        nullptr
    );

    std::string result(directory);
    g_free(directory);

    return result;
}

std::string get_config_path() {
    gchar* path = g_build_filename(
        get_config_directory().c_str(),
        CONFIG_FILENAME,
        nullptr
    );

    std::string result(path);
    g_free(path);

    return result;
}

} // namespace

GKeyFile* load_preferences() {
    GKeyFile* key_file = g_key_file_new();

    const std::string path = get_config_path();

    GError* error = nullptr;

    if (!g_key_file_load_from_file(
            key_file,
            path.c_str(),
            G_KEY_FILE_NONE,
            &error)) {

        if (error != nullptr) {
            g_error_free(error);
        }
    }

    return key_file;
}

std::string get_preference(
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

bool save_preferences(GKeyFile* key_file) {
    const std::string directory = get_config_directory();

    if (g_mkdir_with_parents(directory.c_str(), 0700) != 0) {
        return false;
    }

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

    if (error != nullptr) {
        g_error_free(error);
    }

    g_free(data);

    return saved != FALSE;
}

} // namespace bacus::app
