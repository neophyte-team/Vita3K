#ifndef APPLISTMODEL_H
#define APPLISTMODEL_H

#include "emuenv/state.h"
#include "gui/state.h"
#include "config/state.h"
#include <QAbstractTableModel>
#include <array>

class AppListModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        COLUMN_ICON,
        COLUMN_COMP,
        COLUMN_TITLE_ID,
        COLUMN_VER,
        COLUMN_CAT,
        COLUMN_LAST_TIME,
        COLUMN_NAME,

        COLUMN_COUNT
    };

    explicit AppListModel(GuiState &gui, EmuEnvState &emuenv, std::vector<gui::App> app_selector, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    std::array<QString, COLUMN_COUNT> column_display_names;

private:
    GuiState &gui;
    EmuEnvState &emuenv;

    std::vector<gui::App> app_selector;
};

#endif // APPLISTMODEL_H
