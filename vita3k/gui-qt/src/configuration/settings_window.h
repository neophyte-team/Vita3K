#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include "core_settings_widget.h"
#include "cpu_settings_widget.h"
#include "gpu_settings_widget.h"
#include "system_settings_widget.h"
#include "emulator_settings_widget.h"
#include "network_settings_widget.h"
#include "debug_settings_widget.h"

#include <emuenv/state.h>
#include <gui/state.h>
#include <QWidget>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWindow(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~SettingsWindow();

private:
    Ui::SettingsWindow *ui;

    GuiState &gui;
    EmuEnvState &emuenv;

    CoreSettingsWidget *core_settings = nullptr;
    CpuSettingsWidget *cpu_settings = nullptr;
    GpuSettingsWidget *gpu_settings = nullptr;
    SystemSettingsWidget *system_settings = nullptr;
    EmulatorSettingsWidget *emulator_settings = nullptr;
    NetworkSettingsWidget *network_settings = nullptr;
    DebugSettingsWidget *debug_settings = nullptr;

    void addWidget(QWidget* widget, const QString& title);

private Q_SLOTS:
    void onCategoryCurrentRowChanged(int row);
    void onSaveButtonClicked();
};

#endif // SETTINGS_WINDOW_H
