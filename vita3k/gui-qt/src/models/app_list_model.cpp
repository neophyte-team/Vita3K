#include "app_list_model.h"
#include <gui/functions.h>
#include <QDateTime>
#include <QImage>
#include <QIcon>
#include <QPixmap>
#include <QColorSpace>
#include <QPainter>
#include <utility>

AppListModel::AppListModel(GuiState &gui_, EmuEnvState &emuenv_, QObject *parent)
    : QAbstractTableModel(parent),
    gui{gui_},
    emuenv{emuenv_} {
    refresh_app_list();

    set_column_display_names();
}

QVariant AppListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole || section < 0 || section >= Column::COLUMN_COUNT)
        return {};

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
    case Qt::DecorationRole:
        switch (index.column()) {
        case COLUMN_ICON: {
            QImage icon;

            auto icon_path = fs::path(emuenv.pref_path) / "ux0/app" / entry.path / "sce_sys/icon0.png";
            if (fs::exists(icon_path)) {
                icon.load(QString::fromStdString(icon_path.string()));
                return icon;
            }

            icon_path = fs::path(emuenv.base_path) / "data/image/icon.png";
            if (fs::exists(icon_path)) {
                icon.load(QString::fromStdString(icon_path.string()));
                return icon;
            }

            //TODO: find the way of rendering icon without getting libpng warning: iCCP: known incorrect sRGB profile
            return {};
        }
        case COLUMN_COMP: {
            if (index.row() >= gui.app_selector.sys_apps.size()) {
                const auto has_state_report = gui.compat.compat_db_loaded && gui.compat.app_compat_db.contains(entry.title_id);
                const auto compat_state = has_state_report ? gui.compat.app_compat_db[entry.title_id].state : compat::UNKNOWN;
                const auto compat_state_color = compat_color.at(compat_state);
                auto *pixmap = new QPixmap(64, 64);
                pixmap->fill(Qt::transparent);
                auto *painter = new QPainter(pixmap);
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setBrush(compat_state_color);
                painter->drawEllipse(pixmap->rect());
                painter->end();
                return QIcon(*pixmap);
            }
            return {};
        }
        }
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    }
    return {};
}

void AppListModel::refresh() {
    beginResetModel();
    refresh_app_list();
    endResetModel();
}

void AppListModel::refresh_app_list() {
    app_selector.clear();
    for (auto &app : gui.app_selector.sys_apps)
        app_selector.emplace_back(app);
    for (auto &app : gui.app_selector.user_apps)
        app_selector.emplace_back(app);
}

void AppListModel::set_column_display_names() {
    column_display_names[COLUMN_COMP] = QString("Comp");
    column_display_names[COLUMN_TITLE_ID] = QString("Title ID");
    column_display_names[COLUMN_VER] = QString("Ver");
    column_display_names[COLUMN_CAT] = QString("Cat");
    column_display_names[COLUMN_LAST_TIME] = QString("Last Time");
    column_display_names[COLUMN_NAME] = QString("Name");
}
