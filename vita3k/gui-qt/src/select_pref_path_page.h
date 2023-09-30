#ifndef SELECT_PREF_PATH_PAGE_H
#define SELECT_PREF_PATH_PAGE_H

#include "emuenv/state.h"
#include "gui/state.h"
#include <config/state.h>
#include <QWizardPage>

namespace Ui {
class SelectPrefPathPage;
}

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

#endif // SELECT_PREF_PATH_PAGE_H
