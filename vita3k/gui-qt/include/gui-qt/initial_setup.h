#ifndef INITIAL_SETUP_H
#define INITIAL_SETUP_H

#include "emuenv/state.h"
#include <config/state.h>
#include "gui/state.h"
#include <QWidget>

namespace Ui {
class InitialSetup;
}

class InitialSetup : public QWidget
{
    Q_OBJECT

public:
    explicit InitialSetup(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~InitialSetup();

private:
    Ui::InitialSetup *ui;

    GuiState &gui;
    EmuEnvState &emuenv;
};

#endif // INITIAL_SETUP_H
