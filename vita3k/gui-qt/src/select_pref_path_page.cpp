#include "select_pref_path_page.h"
#include "ui_select_pref_path_page.h"
#include <host/dialog/filesystem.hpp>

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

SelectPrefPathPage::~SelectPrefPathPage() {
    delete ui;
}



