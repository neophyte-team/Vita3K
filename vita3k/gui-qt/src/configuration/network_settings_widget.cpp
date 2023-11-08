#include "network_settings_widget.h"
#include "ui_network_settings_widget.h"

NetworkSettingsWidget::NetworkSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent) :
    BaseSettingsWidget(gui, emuenv, parent),
    ui(new Ui::NetworkSettingsWidget) {
    ui->setupUi(this);

    //TODO: implement this
}

void NetworkSettingsWidget::init_ui() {

}

void NetworkSettingsWidget::init_events() {

}

NetworkSettingsWidget::~NetworkSettingsWidget() {
    delete ui;
}
