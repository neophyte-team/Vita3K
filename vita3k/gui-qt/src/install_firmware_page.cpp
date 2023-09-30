#include "install_firmware_page.h"
#include "config/functions.h"
#include "gui/functions.h"
#include "ui_install_firmware_page.h"
#include <host/dialog/filesystem.hpp>
#include <packages/functions.h>

#include <thread>

std::filesystem::path pup_path = "";

InstallFirmwarePage::InstallFirmwarePage(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::InstallFirmwarePage),
    gui{gui_},
    emuenv{emuenv_} {
    ui->setupUi(this);
}

void InstallFirmwarePage::on_install_firmware_file_button_clicked() {
    static std::mutex install_mutex;
    static bool draw_file_dialog = true;
    static bool finished_installing = false;
    static std::atomic<uint32_t> progress(0);
    static const auto progress_callback = [&](uint32_t updated_progress) {
        progress = updated_progress;
    };
    std::lock_guard<std::mutex> lock(install_mutex);

    auto result = host::dialog::filesystem::open_file(pup_path, { { "PlayStation Vita Firmware Package", { "PUP" } } });
    if (result == host::dialog::filesystem::Result::SUCCESS) {
        std::thread installation([this]() {
            install_pup(emuenv.pref_path, pup_path.string(), progress_callback);
            std::lock_guard<std::mutex> lock(install_mutex);
            finished_installing = true;
            emuenv.cfg.initial_setup = true;
            config::serialize_config(emuenv.cfg, emuenv.base_path);
            gui::get_firmware_version(emuenv);
        });
        installation.detach();
    } else if (result == host::dialog::filesystem::Result::CANCEL) {
        gui.file_menu.firmware_install_dialog = false;
        draw_file_dialog = true;
    } else {
        LOG_ERROR("Error initializing file dialog: {}", host::dialog::filesystem::get_error());
        gui.file_menu.firmware_install_dialog = false;
        draw_file_dialog = true;
    }
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