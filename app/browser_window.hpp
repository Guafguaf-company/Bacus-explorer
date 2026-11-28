#pragma once

#include <gtk/gtk.h>

#include <memory>

namespace bacus::app {

class BrowserWindow {
public:
    explicit BrowserWindow(GtkApplication* application, bool open_homepage = false);
    ~BrowserWindow();

    BrowserWindow(const BrowserWindow&) = delete;
    BrowserWindow& operator=(const BrowserWindow&) = delete;

    void show();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace bacus::app
