#ifndef CPU_SETTINGS_WIDGET_H
#define CPU_SETTINGS_WIDGET_H

#include "base_settings_widget.h"
#include <config/state.h>
#include <emuenv/state.h>
#include <gui/state.h>
#include <QWidget>

namespace Ui {
class CpuSettingsWidget;
}

class CpuSettingsWidget : public BaseSettingsWidget {
    Q_OBJECT

public:
    explicit CpuSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~CpuSettingsWidget();

private:
    Ui::CpuSettingsWidget *ui;

    void init_ui();
    void init_events();
};

#endif // CPU_SETTINGS_WIDGET_H
