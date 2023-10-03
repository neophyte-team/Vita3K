#ifndef LIVE_AREA_H
#define LIVE_AREA_H

#include <QTableView>
#include <QSortFilterProxyModel>
#include <config/state.h>
#include <emuenv/state.h>
#include <gui/state.h>
#include "models/app_list_model.h"
#include <QWidget>

namespace Ui {
class LiveArea;
}

class AppListSortModel final : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit AppListSortModel(AppListModel* parent) :
        QSortFilterProxyModel(parent),
        model(parent) {}

private:
    AppListModel *model;
};

class LiveArea : public QWidget {
    Q_OBJECT

public:
    explicit LiveArea(GuiState &gui, EmuEnvState &emuenv, QWidget *parent = nullptr);
    ~LiveArea();

    void initialize();
    gui::App *get_selected_app();

signals:
    void selection_changed();

private slots:
    void onSelectionModelCurrentChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    Ui::LiveArea *ui;
    AppListModel *model;
    AppListSortModel *sort_model;
    QTableView *table_view;

    GuiState &gui;
    EmuEnvState &emuenv;
    std::vector<gui::App> app_selector;
};

#endif // LIVE_AREA_H
