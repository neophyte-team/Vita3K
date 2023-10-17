#include <emuenv/state.h>
#include <gui/state.h>
#include "main_window.h"
#include <QWizard>

namespace Ui {
class InitialSetupWizard;
}

class InitialSetupWizard : public QWizard {
    Q_OBJECT

public:
    explicit InitialSetupWizard(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~InitialSetupWizard();

    void accept();

private:
    Ui::InitialSetupWizard *ui;

    MainWindow *main_window;

    GuiState &gui;
    EmuEnvState &emuenv;
};
