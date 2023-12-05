#ifndef COMMON_DIALOG_H
#define COMMON_DIALOG_H

#include <emuenv/state.h>
#include <gui/state.h>
#include <QDialog>

namespace Ui {
class MessageDialog;
class ImeDialog;
}

class MessageDialog : public QDialog {
    Q_OBJECT

public:
    explicit MessageDialog(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~MessageDialog();

private:
    Ui::MessageDialog *ui;
};

class ImeDialog : public QDialog {
    Q_OBJECT

public:
    explicit ImeDialog(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~ImeDialog();

private:
    Ui::ImeDialog *ui;
};

#endif // COMMON_DIALOG_H
