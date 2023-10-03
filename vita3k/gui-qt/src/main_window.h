#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "live_area.h"
#include "render_window.h"
#include <QMainWindow>
#include <gui/state.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~MainWindow();

    void on_initial_setup();
    void init_live_area();

private slots:
    void on_actionInstall_pkg_triggered();

private:
    Ui::MainWindow *ui;
    LiveArea *live_area;

    GuiState &gui;
    EmuEnvState &emuenv;

    RenderWindow *render_window;

    void connect_widget_events();
    void start_pkg_installation();
    void on_licence_button();
    void on_app_selection_changed();

};

#endif // MAIN_WINDOW_H
