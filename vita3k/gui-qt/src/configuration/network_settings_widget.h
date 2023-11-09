#ifndef NETWORK_SETTINGS_WIDGET_H
#define NETWORK_SETTINGS_WIDGET_H

#include "base_settings_widget.h"
#include <config/state.h>
#include <emuenv/state.h>
#include <gui/state.h>
#include <QWidget>

namespace Ui {
class NetworkSettingsWidget;
}

class NetworkSettingsWidget : public BaseSettingsWidget {
    Q_OBJECT

public:
    explicit NetworkSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~NetworkSettingsWidget();

private:
    Ui::NetworkSettingsWidget *ui;

    void init_ui();
    void init_events();
};

#endif // NETWORK_SETTINGS_WIDGET_H
