#ifndef SYSTEM_SETTINGS_WIDGET_H
#define SYSTEM_SETTINGS_WIDGET_H

#include "base_settings_widget.h"
#include <config/state.h>
#include <emuenv/state.h>
#include <gui/state.h>
#include <QWidget>

namespace Ui {
class SystemSettingsWidget;
}

class SystemSettingsWidget : public BaseSettingsWidget {
    Q_OBJECT

public:
    explicit SystemSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~SystemSettingsWidget();

private:
    Ui::SystemSettingsWidget *ui;

    void init_ui();
    void init_events();
};

#endif // SYSTEM_SETTINGS_WIDGET_H
