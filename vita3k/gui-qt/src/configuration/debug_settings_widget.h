#ifndef DEBUG_SETTINGS_WIDGET_H
#define DEBUG_SETTINGS_WIDGET_H

#include "base_settings_widget.h"
#include <config/state.h>
#include <emuenv/state.h>
#include <gui/state.h>
#include <QWidget>

namespace Ui {
class DebugSettingsWidget;
}

class DebugSettingsWidget : public BaseSettingsWidget {
    Q_OBJECT

public:
    explicit DebugSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~DebugSettingsWidget();

private:
    Ui::DebugSettingsWidget *ui;

    void init_ui();
    void init_events();
};

#endif // DEBUG_SETTINGS_WIDGET_H
