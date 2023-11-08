#include "initial_setup_wizard.h"

#include <host/dialog/include/host/dialog/filesystem.hpp>
#include <config/include/config/functions.h>
#include <lang/functions.h>
#include <gui/functions.h>
#include <packages/include/packages/functions.h>
#include <QProgressDialog>
#include <QString>
#include <QLineEdit>
#include <thread>

#include "ui_initial_setup_wizard.h"
#include "ui_install_firmware_page.h"
#include "ui_select_language_page.h"
#include "ui_select_pref_path_page.h"

std::filesystem::path pup_path = "";

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

void InitialSetupWizard::accept() {
    emuenv.cfg.initial_setup = true;
    config::serialize_config(emuenv.cfg, emuenv.config_path);

    this->close();
    main_window->init_live_area();
}

SelectLanguagePage::SelectLanguagePage(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::SelectLanguagePage),
    gui{gui_},
    emuenv{emuenv_} {
    ui->setupUi(this);

    for (const auto &sys_lang : LIST_SYS_LANG) {
        const auto is_current_lang = emuenv.cfg.sys_lang == sys_lang.first;
        auto *language_list_widget = new QListWidgetItem();
        language_list_widget->setText(sys_lang.second.c_str());
        language_list_widget->setData(1, sys_lang.first);
        ui->language_list->addItem(language_list_widget);
        if (is_current_lang) {
            ui->language_list->setCurrentItem(language_list_widget);
        }
    }
}

bool SelectLanguagePage::validatePage() {
    emuenv.cfg.sys_lang = ui->language_list->currentItem()->data(1).toInt();
    lang::init_lang(gui.lang, emuenv);
    return true;
}

SelectPrefPathPage::SelectPrefPathPage(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::SelectPrefPathPage),
    gui{gui_},
    emuenv{emuenv_} {
    ui->setupUi(this);

    ui->emulator_path->setText(emuenv.cfg.pref_path.c_str());
}

void SelectPrefPathPage::on_change_emulator_path_clicked() {
    std::filesystem::path emulator_path = "";
    host::dialog::filesystem::Result result = host::dialog::filesystem::pick_folder(emulator_path);

    if ((result == host::dialog::filesystem::Result::SUCCESS) && (emulator_path.wstring() != emuenv.pref_path)) {
        emuenv.pref_path = emulator_path.wstring() + L'/';
        emuenv.cfg.pref_path = emulator_path.string();

        ui->emulator_path->setText(emuenv.cfg.pref_path.c_str());
    }
}

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
    auto result = host::dialog::filesystem::open_file(pup_path, { { "PlayStation Vita Firmware Package", { "PUP" } } });

    if (result == host::dialog::filesystem::Result::SUCCESS) {
        auto progress_dialog = new QProgressDialog(this);
        progress_dialog->setWindowTitle("Installation");
        progress_dialog->setModal(true);
        progress_dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        progress_dialog->setCancelButton(nullptr);

        std::lock_guard<std::mutex> lock(install_mutex);
        std::thread installation([this, &progress_dialog]() {
            auto progress_callback = [&](float updated_progress) {
                QMetaObject::invokeMethod(progress_dialog, "setValue", Qt::QueuedConnection, Q_ARG(int, static_cast<int>(updated_progress)));
            };
            install_pup(emuenv.pref_path.wstring(), pup_path.string(), progress_callback);
            std::lock_guard<std::mutex> lock(install_mutex);

            gui::get_firmware_version(emuenv);
            config::serialize_config(emuenv.cfg, emuenv.config_path);
            initializePage();
        });
        installation.detach();
        progress_dialog->exec();
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

InitialSetupWizard::~InitialSetupWizard() {
    delete ui;
}

SelectLanguagePage::~SelectLanguagePage() {
    delete ui;
}

SelectPrefPathPage::~SelectPrefPathPage() {
    delete ui;
}

InstallFirmwarePage::~InstallFirmwarePage() {
    delete ui;
}
