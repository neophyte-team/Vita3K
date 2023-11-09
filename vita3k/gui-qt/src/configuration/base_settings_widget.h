#ifndef BASE_SETTINGS_WIDGET_H
#define BASE_SETTINGS_WIDGET_H

#include <config/state.h>
#include <emuenv/state.h>
#include <gui/state.h>
#include <QWidget>

namespace Ui {
class BaseSettingsWidget;
}

class BaseSettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit BaseSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);

    void initialize_settings();

private:
    virtual void init_ui() = 0;
    virtual void init_events() = 0;

protected:
    GuiState &gui;
    EmuEnvState &emuenv;
    Config::CurrentConfig &cfg;
};

#endif // BASE_SETTINGS_WIDGET_H
