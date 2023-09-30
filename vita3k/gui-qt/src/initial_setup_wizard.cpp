#include "initial_setup_wizard.h"
#include "ui_initial_setup_wizard.h"
#include "select_language_page.h"
#include "select_pref_path_page.h"
#include "install_firmware_page.h"

InitialSetupWizard::InitialSetupWizard(GuiState &gui, EmuEnvState &emuenv, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::InitialSetupWizard) {

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

}
