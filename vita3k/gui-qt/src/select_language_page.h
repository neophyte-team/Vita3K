#ifndef SELECT_LANGUAGE_PAGE_H
#define SELECT_LANGUAGE_PAGE_H

#include "emuenv/state.h"
#include "gui/state.h"
#include <config/state.h>
#include <QWizardPage>

namespace Ui {
class SelectLanguagePage;
}

class SelectLanguagePage : public QWizardPage {
    Q_OBJECT

public:
    explicit SelectLanguagePage(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~SelectLanguagePage();

private:
    Ui::SelectLanguagePage *ui;
    QPushButton *next_button;

    GuiState &gui;
    EmuEnvState &emuenv;

    bool validatePage() override;
};

#endif // SELECT_LANGUAGE_PAGE_H
