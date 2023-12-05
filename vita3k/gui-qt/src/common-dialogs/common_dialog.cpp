#include "common_dialog.h"
#include "ui_ime_dialog.h"
#include "ui_message_dialog.h"
#include <util/string_utils.h>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

MessageDialog::MessageDialog(GuiState &gui, EmuEnvState &emuenv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageDialog) {
    ui->setupUi(this);

    setModal(true);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);

    ui->message->setText(QString::fromStdString(emuenv.common_dialog.msg.message));

    if (emuenv.common_dialog.msg.has_progress_bar) {
        auto progress_bar = new QProgressBar(this);
        progress_bar->setValue(static_cast<int>(static_cast<float>(emuenv.common_dialog.msg.bar_percent) / 100.f));

        ui->verticalLayout->addWidget(progress_bar);
    }

    if (emuenv.common_dialog.msg.btn_num != 0) {
        for (int i = 0; i < emuenv.common_dialog.msg.btn_num; i++) {
            auto button = new QPushButton(QString::fromStdString(emuenv.common_dialog.msg.btn[i]), this);
            connect(button, &QPushButton::released, this, [=, this, &emuenv]() {
                emuenv.common_dialog.msg.status = emuenv.common_dialog.msg.btn_val[i];
                emuenv.common_dialog.result = SCE_COMMON_DIALOG_RESULT_OK;
                emuenv.common_dialog.status = SCE_COMMON_DIALOG_STATUS_FINISHED;
                close();
            });
            ui->horizontalLayout->addWidget(button);
        }
    }
}

ImeDialog::ImeDialog(GuiState &gui, EmuEnvState &emuenv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImeDialog) {
    ui->setupUi(this);

    setModal(true);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);

    ui->title->setText(emuenv.common_dialog.ime.title);

    if (emuenv.common_dialog.ime.multiline) {
        auto text_edit = new QPlainTextEdit(this);
        text_edit->setPlainText(emuenv.common_dialog.ime.text);
        //TODO: implement setMaxLength

        ui->verticalLayout->addWidget(text_edit);
    } else {
        auto line_edit = new QLineEdit(this);
        line_edit->setText(emuenv.common_dialog.ime.text);
        line_edit->setMaxLength(static_cast<int>(emuenv.common_dialog.ime.max_length + 1));

        ui->verticalLayout->addWidget(line_edit);
    }

    auto submit_button = new QPushButton(QString::fromStdString(emuenv.common_dialog.lang.common["submit"]), this);
    connect(submit_button, &QPushButton::released, this, [=, this, &emuenv]() {
        emuenv.common_dialog.ime.status = SCE_IME_DIALOG_BUTTON_ENTER;
        emuenv.common_dialog.status = SCE_COMMON_DIALOG_STATUS_FINISHED;
        emuenv.common_dialog.result = SCE_COMMON_DIALOG_RESULT_OK;
        const std::u16string result16 = string_utils::utf8_to_utf16(emuenv.common_dialog.ime.text);
        memcpy(emuenv.common_dialog.ime.result, result16.c_str(), (result16.length() + 1) * sizeof(uint16_t));
        close();
    });
    ui->verticalLayout->addWidget(submit_button);

    if (emuenv.common_dialog.ime.cancelable) {
        auto cancel_button = new QPushButton(QString::fromStdString(emuenv.common_dialog.lang.common["cancel"]), this);
        connect(cancel_button, &QPushButton::released, this, [=, this, &emuenv]() {
            emuenv.common_dialog.ime.status = SCE_IME_DIALOG_BUTTON_CLOSE;
            emuenv.common_dialog.status = SCE_COMMON_DIALOG_STATUS_FINISHED;
            emuenv.common_dialog.result = SCE_COMMON_DIALOG_RESULT_USER_CANCELED;
            close();
        });
        ui->verticalLayout->addWidget(cancel_button);
    }
}

MessageDialog::~MessageDialog() {
    delete ui;
}

ImeDialog::~ImeDialog() {
    delete ui;
}
