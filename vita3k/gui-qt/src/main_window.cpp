#include "main_window.h"
#include "ui_main_window.h"
#include "initial_setup_wizard.h"

MainWindow::MainWindow(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gui{gui_},
    emuenv{emuenv_} {
    ui->setupUi(this);

    connect_widget_events();
}

void MainWindow::init_live_area() {
    for (auto &app : gui.app_selector.sys_apps) {
        LOG_ERROR("{}", app.title);
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::connect_widget_events() {
}

void MainWindow::on_initial_setup() {
    auto *initial_setup_wizard = new InitialSetupWizard(gui, emuenv);
    initial_setup_wizard->show();
    this->close();
}
