#ifndef INSTALL_FIRMWARE_PAGE_H
#define INSTALL_FIRMWARE_PAGE_H

#include "emuenv/state.h"
#include "gui/state.h"
#include <config/state.h>
#include <QWizardPage>

namespace Ui {
class InstallFirmwarePage;
}

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
};

#endif // INSTALL_FIRMWARE_PAGE_H
