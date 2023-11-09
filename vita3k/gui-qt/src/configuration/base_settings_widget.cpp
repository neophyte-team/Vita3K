#include "base_settings_widget.h"

BaseSettingsWidget::BaseSettingsWidget(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QWidget(parent),
    gui{gui_},
    emuenv{emuenv_},
    cfg{emuenv_.cfg.current_config} {}

void BaseSettingsWidget::initialize_settings() {
    init_ui();
    init_events();
}
