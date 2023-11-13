#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "live_area.h"
#include "render_window.h"
#include <QTimer>
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

    GuiState &gui;
    EmuEnvState &emuenv;

    void on_initial_setup();
    void init_live_area();

private slots:
    void on_actionInstall_Firmware_triggered();
    void on_actionInstall_pkg_triggered();
    void on_actionSettings_triggered();
    void on_actionUser_Management_triggered();

private:
    Ui::MainWindow *ui;
    LiveArea *live_area;

    RenderWindow *render_window;

    QDialog *pkg_installation_dialog;

    QTimer update_input_timer;

    void start_pkg_installation();
    void on_licence_button();
    void on_zrif_button();
    void on_app_selection_changed();

    void update_input();

};

#endif // MAIN_WINDOW_H
