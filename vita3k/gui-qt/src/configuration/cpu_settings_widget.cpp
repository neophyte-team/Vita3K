#include "cpu_settings_widget.h"
#include "ui_cpu_settings_widget.h"

static const char *LIST_CPU_BACKEND[] = { "Dynarmic", "Unicorn" };
static const char *LIST_CPU_BACKEND_DISPLAY[] = { "Dynarmic", "Unicorn (deprecated)" };

CpuSettingsWidget::CpuSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent) :
    BaseSettingsWidget(gui, emuenv, parent),
    ui(new Ui::CpuSettingsWidget) {
    ui->setupUi(this);

    initialize_settings();
}

void CpuSettingsWidget::init_ui() {
    auto current_cpu_backend = 0;
    for (const auto backend : LIST_CPU_BACKEND_DISPLAY) {
        ui->cpu_backend->addItem(backend);
        if (cfg.cpu_backend == backend)
            ui->cpu_backend->setCurrentIndex(current_cpu_backend);
        current_cpu_backend++;
    }

    ui->enable_optimizations->setChecked(cfg.cpu_opt);
}

void CpuSettingsWidget::init_events() {
    connect(ui->cpu_backend, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        cfg.cpu_backend = LIST_CPU_BACKEND[index];
    });

    connect(ui->enable_optimizations, &QCheckBox::stateChanged, this, [this]() {
        cfg.cpu_opt = ui->enable_optimizations->isChecked();
    });
}

CpuSettingsWidget::~CpuSettingsWidget() {
    delete ui;
}
