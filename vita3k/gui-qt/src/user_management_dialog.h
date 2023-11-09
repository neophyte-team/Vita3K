#ifndef USER_MANAGEMENT_DIALOG_H
#define USER_MANAGEMENT_DIALOG_H

#include <emuenv/state.h>
#include <gui/state.h>
#include <QDialog>
#include <QItemDelegate>
#include <QAbstractButton>

namespace Ui {
class UserManagementDialog;
}

class UserManagementDialog : public QDialog {
    Q_OBJECT

public:
    explicit UserManagementDialog(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~UserManagementDialog();

private slots:
    void on_button_box_clicked(QAbstractButton *button);

private:
    Ui::UserManagementDialog *ui;

    GuiState &gui;
    EmuEnvState &emuenv;

    void update_users_list();
    void on_create_user_button();
    void on_delete_user_button();
};

class UserItemDelegate : public QItemDelegate {
    Q_OBJECT

public:
    explicit UserItemDelegate(GuiState &gui, EmuEnvState &emuenv, QObject *parent = 0);

protected:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
    void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const;

private:
    GuiState &gui;
    EmuEnvState &emuenv;
};

#endif // USER_MANAGEMENT_DIALOG_H
