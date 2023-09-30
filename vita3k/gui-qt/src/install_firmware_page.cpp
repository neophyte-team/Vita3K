#include "install_firmware_page.h"
#include "ui_install_firmware_page.h"
#include <host/dialog/filesystem.hpp>
#include <gui/functions.h>
#include <packages/functions.h>
#include <QProgressBar>

#include <thread>

std::filesystem::path pup_path = "";

InstallFirmwarePage::InstallFirmwarePage(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallFirmwarePage),
    gui{gui_},
    emuenv{emuenv_} {
    ui->setupUi(this);

    ui->installed_firmware_status->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->installed_firmware_status->setFocusPolicy(Qt::NoFocus);
    ui->installed_fonts_status->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->installed_fonts_status->setFocusPolicy(Qt::NoFocus);
}

void InstallFirmwarePage::on_install_firmware_file_button_clicked() {
    static std::mutex install_mutex;
    static std::atomic<uint32_t> progress(0);
    static const auto progress_callback = [&](uint32_t updated_progress) {
        progress = updated_progress;
    };
    std::lock_guard<std::mutex> lock(install_mutex);

    auto result = host::dialog::filesystem::open_file(pup_path, { { "PlayStation Vita Firmware Package", { "PUP" } } });
    if (result == host::dialog::filesystem::Result::SUCCESS) {
        std::thread installation([this]() {
            install_pup(emuenv.pref_path.wstring(), pup_path.string(), progress_callback);
            std::lock_guard<std::mutex> lock(install_mutex);

            //TODO: show progress dialog
            LOG_ERROR("INSTALLED");
            gui::get_firmware_version(emuenv);
            initializePage();
        });
        installation.detach();
    }
}

void InstallFirmwarePage::initializePage() {
    const auto FW_PATH{ emuenv.pref_path / "vs0" };
    const auto FW_FONT_PATH{ emuenv.pref_path / "sa0" };
    const auto FW_INSTALLED = fs::exists(FW_PATH) && !fs::is_empty(FW_PATH);
    const auto FW_FONT_INSTALLED = fs::exists(FW_FONT_PATH) && !fs::is_empty(FW_FONT_PATH);

    ui->installed_firmware_status->setChecked(FW_INSTALLED);
    ui->installed_fonts_status->setChecked(FW_FONT_INSTALLED);
}

void InstallFirmwarePage::on_download_firmware_button_clicked() {
    gui::get_firmware_file(emuenv);
}


void InstallFirmwarePage::on_download_font_package_button_clicked() {
    gui::open_path("https://bit.ly/2P2rb0r");
}

InstallFirmwarePage::~InstallFirmwarePage() {
    delete ui;
}