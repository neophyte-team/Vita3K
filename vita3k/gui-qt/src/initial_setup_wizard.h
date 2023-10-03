#include "emuenv/state.h"
#include "gui/state.h"
#include <QWizard>

namespace Ui {
class InitialSetupWizard;
}

class InitialSetupWizard : public QWizard {
    Q_OBJECT

public:
    explicit InitialSetupWizard(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~InitialSetupWizard();

    void accept() override;

private:
    Ui::InitialSetupWizard *ui;
};
