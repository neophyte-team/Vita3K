#include "initial_setup_wizard.h"
#include "ui_initial_setup_wizard.h"
#include "select_language_page.h"
#include "select_pref_path_page.h"
#include "install_firmware_page.h"
#include <config/functions.h>

InitialSetupWizard::InitialSetupWizard(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    ui(new Ui::InitialSetupWizard),
    main_window{dynamic_cast<MainWindow *>(parent)},
    gui{gui_},
    emuenv{emuenv_} {

    QList<QWizard::WizardButton> layout;
    layout << QWizard::BackButton << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);
    ui->setupUi(this);

    addPage(new SelectLanguagePage(gui, emuenv));
    addPage(new SelectPrefPathPage(gui, emuenv));
    addPage(new InstallFirmwarePage(gui, emuenv));
}

InitialSetupWizard::~InitialSetupWizard() {
    delete ui;
}

void InitialSetupWizard::accept() {
    emuenv.cfg.initial_setup = true;
    config::serialize_config(emuenv.cfg, emuenv.base_path);

    this->close();
    main_window->init_live_area();
}
