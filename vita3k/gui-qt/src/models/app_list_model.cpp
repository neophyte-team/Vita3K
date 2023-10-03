#include "app_list_model.h"
#include <QDateTime>
#include <utility>

AppListModel::AppListModel(GuiState &gui_, EmuEnvState &emuenv_, std::vector<gui::App> app_selector_, QObject *parent)
    : QAbstractTableModel(parent),
    gui{gui_},
    emuenv{emuenv_},
    app_selector{std::move(app_selector_)} {
}

QVariant AppListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return column_display_names[section];
}

int AppListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return static_cast<int>(app_selector.size());
}

int AppListModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return COLUMN_COUNT;
}

QVariant AppListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};

    auto entry = app_selector.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case COLUMN_COMP:
            return entry.compat;
        case COLUMN_TITLE_ID:
            return QString::fromStdString(entry.title_id);
        case COLUMN_VER:
            return QString::fromStdString(entry.app_ver);
        case COLUMN_CAT:
            return QString::fromStdString(entry.category);
        case COLUMN_LAST_TIME:
            return QDateTime::fromTime_t(entry.last_time);
        case COLUMN_NAME:
            return QString::fromStdString(entry.title);
        }
    }
    return {};
}
