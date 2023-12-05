#include "user_management_dialog.h"
#include "ui_user_management_dialog.h"
#include <gui-qt/functions.h>
#include <config/functions.h>
#include <config/state.h>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>

User selected_user;

UserManagementDialog::UserManagementDialog(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserManagementDialog),
    gui{gui_},
    emuenv{emuenv_} {
    ui->setupUi(this);
    this->setWindowTitle("User Management");

    ui->button_box->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(true);
    ui->delete_user_button->setDisabled(true);
    ui->automatic_user_login->setChecked(emuenv.cfg.auto_user_login);

    connect(ui->create_user_button, &QPushButton::released, this, &UserManagementDialog::on_create_user_button);
    connect(ui->delete_user_button, &QPushButton::released, this, &UserManagementDialog::on_delete_user_button);

    connect(ui->automatic_user_login, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.auto_user_login = ui->automatic_user_login->isChecked();
        config::serialize_config(emuenv.cfg, emuenv.cfg.config_path);
    });

    connect(ui->users_list, &QListWidget::itemClicked, this, [this]() {
        selected_user = gui.users[ui->users_list->currentItem()->data(1).toString().toStdString()];
        ui->delete_user_button->setDisabled(false);
        ui->button_box->button(QDialogButtonBox::StandardButton::Ok)->setDisabled(false);
    });

    update_users_list();

    auto user_item_delegate = new UserItemDelegate(gui, emuenv);
    ui->users_list->setItemDelegate(user_item_delegate);
}

void UserManagementDialog::on_create_user_button() {
    auto new_user = gui_qt::create_temp_user(gui, emuenv);
    gui_qt::save_user(gui, emuenv, new_user);

    update_users_list();
}

void UserManagementDialog::on_delete_user_button() {
    auto reply = QMessageBox::question(this, "Delete User", "Are you sure you want to delete this user?",
        QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        gui_qt::delete_user(gui, emuenv, selected_user);

        update_users_list();
        ui->delete_user_button->setDisabled(true);
    }
}

void UserManagementDialog::update_users_list() {
    ui->users_list->clear();

    gui_qt::get_users_list(gui, emuenv);
    for (const auto &user : gui.users) {
        auto *users_list_widget = new QListWidgetItem();
        users_list_widget->setFlags(users_list_widget->flags() | Qt::ItemIsEditable);
        users_list_widget->setText(QString::fromStdString(user.second.name));
        users_list_widget->setData(1, QString::fromStdString(user.second.id));
        ui->users_list->addItem(users_list_widget);
    }
}

UserItemDelegate::UserItemDelegate(GuiState &gui_, EmuEnvState &emuenv_, QObject *parent) :
    QItemDelegate(parent),
    gui{gui_},
    emuenv{emuenv_} {
}

QWidget *UserItemDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
    auto *editor = new QLineEdit(parent);
    return editor;
}


void UserItemDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const {
    QString value =index.model()->data(index, Qt::EditRole).toString();
    auto *line = dynamic_cast<QLineEdit*>(editor);
    line->setText(value);
}


void UserItemDelegate::setModelData(QWidget *editor,
    QAbstractItemModel *model,
    const QModelIndex &index) const {
    auto *line = dynamic_cast<QLineEdit*>(editor);
    QString value = line->text();

    for (const auto &user : gui.users) {
        if (user.second.name == value.toStdString()) {
            return;
        }
    }

    auto user = gui.users[fmt::format("{:0>2d}", index.row())];
    user.name = value.toStdString();
    gui_qt::save_user(gui, emuenv, user);

    model->setData(index, value);
}


void UserItemDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
    editor->setGeometry(option.rect);
}

void UserManagementDialog::on_button_box_clicked(QAbstractButton *button) {
    if (button == ui->button_box->button(QDialogButtonBox::Ok)) {
        gui_qt::select_user(gui, emuenv, selected_user);
        this->close();
    }
}

UserManagementDialog::~UserManagementDialog() {
    delete ui;
}
