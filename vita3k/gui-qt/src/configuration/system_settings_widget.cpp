#include "system_settings_widget.h"
#include "ui_system_settings_widget.h"

SystemSettingsWidget::SystemSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent) :
    BaseSettingsWidget(gui, emuenv, parent),
    ui(new Ui::SystemSettingsWidget) {
    ui->setupUi(this);

    initialize_settings();
}

void SystemSettingsWidget::init_ui() {
    switch (emuenv.cfg.sys_button) {
    case 0:
        ui->circle->setChecked(true);
        break;
    case 1:
        ui->cross->setChecked(true);
        break;
    }

    ui->pstv_mode->setChecked(cfg.pstv_mode);
    ui->show_mode->setChecked(emuenv.cfg.show_mode);
    ui->demo_mode->setChecked(emuenv.cfg.demo_mode);
}

void SystemSettingsWidget::init_events() {
    connect(ui->circle, &QRadioButton::clicked, this, [this]() {
        emuenv.cfg.sys_button = 0;
    });

    connect(ui->cross, &QRadioButton::clicked, this, [this]() {
        emuenv.cfg.sys_button = 1;
    });

    connect(ui->pstv_mode, &QCheckBox::stateChanged, this, [this]() {
        cfg.pstv_mode = ui->pstv_mode->isChecked();
    });

    connect(ui->show_mode, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.show_mode = ui->show_mode->isChecked();
    });

    connect(ui->demo_mode, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.demo_mode = ui->demo_mode->isChecked();
    });
}

SystemSettingsWidget::~SystemSettingsWidget() {
    delete ui;
}
