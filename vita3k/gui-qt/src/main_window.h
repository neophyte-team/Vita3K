#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "gui/state.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~MainWindow();

    void on_initial_setup();
    void init_live_area();

private:
    Ui::MainWindow *ui;
    GuiState &gui;
    EmuEnvState &emuenv;

    void connect_widget_events();
};

#endif // MAIN_WINDOW_H
