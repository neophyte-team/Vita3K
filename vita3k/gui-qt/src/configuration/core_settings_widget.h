#ifndef CORE_SETTINGS_WIDGET_H
#define CORE_SETTINGS_WIDGET_H

#include "base_settings_widget.h"
#include <QWidget>
#include <config/state.h>
#include <emuenv/state.h>
#include <gui/state.h>

namespace Ui {
class CoreSettingsWidget;
}

class CoreSettingsWidget : public BaseSettingsWidget {
    Q_OBJECT

public:
    explicit CoreSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~CoreSettingsWidget();

private:
    Ui::CoreSettingsWidget *ui;

    void init_ui();
    void init_events();
};

#endif // CORE_SETTINGS_WIDGET_H
