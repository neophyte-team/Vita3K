#include "live_area.h"
#include <QHeaderView>
#include <QAbstractProxyModel>
#include <ui_live_area.h>
#include "models/app_list_model.h"

LiveArea::LiveArea(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiveArea),
    gui{gui_},
    emuenv{emuenv_} {
    for (auto &app : gui.app_selector.sys_apps)
        app_selector.emplace_back(app);
    for (auto &app : gui.app_selector.user_apps)
        app_selector.emplace_back(app);
}

void LiveArea::initialize() {
    ui->setupUi(this);

    init_table_view();

    connect(table_view->selectionModel(), &QItemSelectionModel::currentChanged, this, &LiveArea::onSelectionModelCurrentChanged);
    connect(table_view, &QTableView::customContextMenuRequested, this, &LiveArea::onTableViewHeaderContextMenuRequested);
    connect(table_view->horizontalHeader(), &QHeaderView::customContextMenuRequested, this, &LiveArea::onTableViewHeaderContextMenuRequested);
}

void LiveArea::init_table_view() {
    model = new AppListModel(gui, emuenv, this);
    sort_model = new AppListSortModel(model);
    sort_model->setSourceModel(model);

    table_view = new QTableView(ui->stack);
    table_view->setModel(sort_model);
    table_view->setSortingEnabled(true);
    table_view->verticalHeader()->setDefaultSectionSize(85);
    table_view->setSelectionMode(QAbstractItemView::SingleSelection);
    table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_view->setContextMenuPolicy(Qt::CustomContextMenu);
    table_view->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    table_view->setShowGrid(true);
    table_view->setCurrentIndex({});
    table_view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table_view->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    table_view->verticalHeader()->hide();

    // TODO: find the way to center all items (why it's not centered after changing section resize mode?)
    table_view->horizontalHeader()->setSectionResizeMode(AppListModel::Column::COLUMN_ICON, QHeaderView::Fixed);
    table_view->horizontalHeader()->setSectionResizeMode(AppListModel::Column::COLUMN_COMP, QHeaderView::ResizeToContents);
    table_view->horizontalHeader()->setSectionResizeMode(AppListModel::Column::COLUMN_TITLE_ID, QHeaderView::Fixed);
    table_view->horizontalHeader()->setSectionResizeMode(AppListModel::Column::COLUMN_VER, QHeaderView::Fixed);
    table_view->horizontalHeader()->setSectionResizeMode(AppListModel::Column::COLUMN_CAT, QHeaderView::Fixed);
    table_view->horizontalHeader()->setSectionResizeMode(AppListModel::Column::COLUMN_LAST_TIME, QHeaderView::Fixed);
    table_view->horizontalHeader()->setSectionResizeMode(AppListModel::Column::COLUMN_NAME, QHeaderView::Stretch);

    ui->stack->insertWidget(0, table_view);
}

gui::App* LiveArea::get_selected_app() {
    if (ui->stack->currentIndex() == 0) {
        const auto selection_model = table_view->selectionModel();

        if (!selection_model->hasSelection())
            return nullptr;

        const auto selected_rows = selection_model->selectedRows();
        if (selected_rows.empty())
            return nullptr;

        const auto source_index = sort_model->mapToSource(selected_rows[0]);
        if (!source_index.isValid())
            return nullptr;

        return &app_selector[source_index.row()];
    }
}

void LiveArea::onSelectionModelCurrentChanged(const QModelIndex& current, const QModelIndex& previous) {
    const auto source_index = sort_model->mapToSource(current);
    if (!source_index.isValid() || source_index.row() >= static_cast<int>(gui.app_selector.user_apps.size() + gui.app_selector.sys_apps.size()))
        return;

    emit selection_changed();
}

void LiveArea::refresh_app_list() {
    model->refresh();
}

void LiveArea::onTableViewHeaderContextMenuRequested(const QPoint& point) {
    // TODO: implement it
}

LiveArea::~LiveArea() {
    delete ui;
}

