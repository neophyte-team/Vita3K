#include "core_settings_widget.h"
#include "ui_core_settings_widget.h"

CoreSettingsWidget::CoreSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent) :
    BaseSettingsWidget(gui, emuenv, parent),
    ui(new Ui::CoreSettingsWidget) {
    ui->setupUi(this);

    initialize_settings();
}

void CoreSettingsWidget::init_ui() {
    ui->automatic->setToolTip(config_modules_mode[0][ModulesModeType::DESCRIPTION]);
    ui->auto_and_manual->setToolTip(config_modules_mode[1][ModulesModeType::DESCRIPTION]);
    ui->manual->setToolTip(config_modules_mode[2][ModulesModeType::DESCRIPTION]);

    switch (cfg.modules_mode) {
    case ModulesMode::AUTOMATIC:
        ui->modules_list->setDisabled(true);
        ui->automatic->setChecked(true);
        break;
    case ModulesMode::AUTO_MANUAL:
        ui->auto_and_manual->setChecked(true);
        break;
    case ModulesMode::MANUAL:
        ui->manual->setChecked(true);
        break;
    }

    ui->modules_list->setSelectionMode(QAbstractItemView::MultiSelection);

    for (auto &m : gui.modules) {
        const auto module = std::find(cfg.lle_modules.begin(), cfg.lle_modules.end(), m.first);
        const bool module_existed = (module != cfg.lle_modules.end());

        auto *modules_list_widget = new QListWidgetItem();
        modules_list_widget->setText(QString::fromStdString(m.first));
        ui->modules_list->addItem(modules_list_widget);
        modules_list_widget->setSelected(module_existed);
    }
}

void CoreSettingsWidget::init_events() {
    connect(ui->automatic, &QRadioButton::clicked, this, [this]() {
        cfg.modules_mode = ModulesMode::AUTOMATIC;
        ui->modules_list->setDisabled(true);
        cfg.lle_modules.clear();
    });
    connect(ui->auto_and_manual, &QRadioButton::clicked, this, [this]() {
        cfg.modules_mode = ModulesMode::AUTO_MANUAL;
        ui->modules_list->setDisabled(false);
    });
    connect(ui->manual, &QRadioButton::clicked, this, [this]() {
        cfg.modules_mode = ModulesMode::MANUAL;
        ui->modules_list->setDisabled(false);
    });

    connect(ui->modules_list, &QListWidget::itemSelectionChanged, this, [this]() {
        const auto selected_module = gui.modules[ui->modules_list->currentRow()];
        const auto module = std::find(cfg.lle_modules.begin(), cfg.lle_modules.end(),
            selected_module.first);
        const bool module_existed = (module != cfg.lle_modules.end());
        if (module_existed)
            cfg.lle_modules.erase(module);
        else
            cfg.lle_modules.push_back(selected_module.first);
    });
}

CoreSettingsWidget::~CoreSettingsWidget() {
    delete ui;
}
