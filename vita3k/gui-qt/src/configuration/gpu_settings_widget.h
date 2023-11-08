#ifndef GPU_SETTINGS_WIDGET_H
#define GPU_SETTINGS_WIDGET_H

#include "base_settings_widget.h"
#include <config/state.h>
#include <emuenv/state.h>
#include <gui/state.h>
#include <QWidget>

namespace Ui {
class GpuSettingsWidget;
}

class GpuSettingsWidget : public BaseSettingsWidget {
    Q_OBJECT

public:
    explicit GpuSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~GpuSettingsWidget();

private:
    Ui::GpuSettingsWidget *ui;

    void init_ui();
    void init_events();
};

#endif // GPU_SETTINGS_WIDGET_H
