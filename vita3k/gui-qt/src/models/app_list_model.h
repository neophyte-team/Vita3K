#ifndef APPLISTMODEL_H
#define APPLISTMODEL_H

#include "emuenv/state.h"
#include "gui/state.h"
#include "config/state.h"
#include <QAbstractTableModel>
#include <array>
#include <QColor>

class AppListModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        COLUMN_ICON,
        COLUMN_TITLE_ID,
        COLUMN_VER,
        COLUMN_CAT,
        COLUMN_LAST_TIME,
        COLUMN_NAME,

        COLUMN_COUNT
    };

    std::map<compat::CompatibilityState, QColor> compat_color{
        { compat::UNKNOWN, QColor(134, 134, 134) }, // #868686
        { compat::NOTHING, QColor(255, 0, 0) }, // #ff0000
        { compat::BOOTABLE, QColor(98, 31, 165) }, // #621fa5
        { compat::INTRO, QColor(199, 21, 133) }, // #c71585
        { compat::MENU, QColor(29, 118, 219) }, // #1d76db
        { compat::INGAME_LESS, QColor(224, 138, 30) }, // #e08a1e
        { compat::INGAME_MORE, QColor(255, 215, 0) }, // #ffd700
        { compat::PLAYABLE, QColor(14, 138, 22) }, // #0e8a16
    };

    explicit AppListModel(GuiState &gui, EmuEnvState &emuenv, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void refresh();
    void refresh_app_list();

private:
    GuiState &gui;
    EmuEnvState &emuenv;

    std::vector<gui::App> app_selector;
    std::array<QString, COLUMN_COUNT> column_display_names;

    void set_column_display_names();
};

#endif // APPLISTMODEL_H
