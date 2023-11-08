#include "settings_window.h"
#include "ui_settings_window.h"
#include <config/functions.h>
#include <gui/functions.h>
#include <gui-qt/functions.h>
#include <io/state.h>

SettingsWindow::SettingsWindow(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow),
    gui{gui_},
    emuenv{emuenv_} {
    ui->setupUi(this);

    this->setWindowTitle(QString("Settings"));

    addWidget(core_settings = new CoreSettingsWidget(gui, emuenv, ui->settings_container), QString("Core"));
    addWidget(cpu_settings = new CpuSettingsWidget(gui, emuenv, ui->settings_container), QString("CPU"));
    addWidget(gpu_settings = new GpuSettingsWidget(gui, emuenv, ui->settings_container), QString("GPU"));
    addWidget(system_settings = new SystemSettingsWidget(gui, emuenv, ui->settings_container), QString("System"));
    addWidget(emulator_settings = new EmulatorSettingsWidget(gui, emuenv, ui->settings_container), QString("Emulator"));
    addWidget(network_settings = new NetworkSettingsWidget(gui, emuenv, ui->settings_container), QString("Network"));
    addWidget(debug_settings = new DebugSettingsWidget(gui, emuenv, ui->settings_container), QString("Debug"));

    ui->settings_category->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    ui->settings_category->setCurrentRow(0);

    connect(ui->settings_category, &QListWidget::currentRowChanged, this, &SettingsWindow::onCategoryCurrentRowChanged);
    connect(ui->save_button, &QPushButton::clicked, this, &SettingsWindow::onSaveButtonClicked);
}

void SettingsWindow::addWidget(QWidget* widget, const QString &title) {
    auto *item = new QListWidgetItem(ui->settings_category);
    item->setText(title);

    ui->settings_container->addWidget(widget);
}

void SettingsWindow::onCategoryCurrentRowChanged(int row) {
    ui->settings_container->setCurrentIndex(row);
}

void SettingsWindow::onSaveButtonClicked() {
    gui_qt::save_config(gui, emuenv, emuenv.cfg.current_config);
    gui::set_config(gui, emuenv, emuenv.io.app_path);
}

SettingsWindow::~SettingsWindow() {
    delete ui;
}
