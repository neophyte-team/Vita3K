#include "select_language_page.h"
#include "lang/functions.h"
#include "ui_select_language_page.h"


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


SelectLanguagePage::~SelectLanguagePage() {
    delete ui;
}
