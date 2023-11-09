#ifndef EMULATOR_SETTINGS_WIDGET_H
#define EMULATOR_SETTINGS_WIDGET_H

#include "base_settings_widget.h"
#include <config/state.h>
#include <emuenv/state.h>
#include <gui/state.h>
#include <QWidget>

namespace Ui {
class EmulatorSettingsWidget;
}

class EmulatorSettingsWidget : public BaseSettingsWidget {
    Q_OBJECT

public:
    explicit EmulatorSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~EmulatorSettingsWidget();

private:
    Ui::EmulatorSettingsWidget *ui;

    void init_ui();
    void init_events();
};

#endif // EMULATOR_SETTINGS_WIDGET_H
