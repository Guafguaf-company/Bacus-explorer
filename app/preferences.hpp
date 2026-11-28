#pragma once

#include <string>

#include <glib.h>

namespace bacus::app {

GKeyFile* load_preferences();

std::string get_preference(
    GKeyFile* key_file,
    const char* group,
    const char* key,
    const char* fallback
);

bool save_preferences(GKeyFile* key_file);

} // namespace bacus::app
