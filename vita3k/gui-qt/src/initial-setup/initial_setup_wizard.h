#include <src/main_window.h>
#include <emuenv/include/emuenv/state.h>
#include <gui/include/gui/state.h>
#include <config/include/config/state.h>
#include <emuenv/state.h>
#include <gui/state.h>
#include <QWizard>
#include <QWizardPage>

namespace Ui {
class InitialSetupWizard;
class SelectLanguagePage;
class SelectPrefPathPage;
class InstallFirmwarePage;
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

class SelectLanguagePage : public QWizardPage {
    Q_OBJECT

public:
    explicit SelectLanguagePage(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~SelectLanguagePage();

private:
    Ui::SelectLanguagePage *ui;

    GuiState &gui;
    EmuEnvState &emuenv;

    bool validatePage() override;
};

class SelectPrefPathPage : public QWizardPage {
    Q_OBJECT

public:
    explicit SelectPrefPathPage(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~SelectPrefPathPage();

private slots:
    void on_change_emulator_path_clicked();

private:
    Ui::SelectPrefPathPage *ui;

    GuiState &gui;
    EmuEnvState &emuenv;
};

class InstallFirmwarePage : public QWizardPage {
    Q_OBJECT

public:
    explicit InstallFirmwarePage(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~InstallFirmwarePage();

private slots:
    void on_install_firmware_file_button_clicked();
    void on_download_firmware_button_clicked();
    void on_download_font_package_button_clicked();

private:
    Ui::InstallFirmwarePage *ui;

    GuiState &gui;
    EmuEnvState &emuenv;

    void initializePage() override;
};
