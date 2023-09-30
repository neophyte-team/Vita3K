#ifndef INITIAL_SETUP_WIZARD_H
#define INITIAL_SETUP_WIZARD_H

#include "emuenv/state.h"
#include "gui/state.h"
#include <QWizard>

namespace Ui {
class InitialSetupWizard;
}

class InitialSetupWizard : public QWizard
{
    Q_OBJECT

public:
    explicit InitialSetupWizard(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~InitialSetupWizard();

    void accept() override;

private:
    Ui::InitialSetupWizard *ui;
};

#endif // INITIAL_SETUP_WIZARD_H
