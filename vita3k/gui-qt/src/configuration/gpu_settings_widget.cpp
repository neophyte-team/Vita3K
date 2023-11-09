#include "gpu_settings_widget.h"
#include "ui_gpu_settings_widget.h"
#include <renderer/state.h>

static const char *LIST_BACKEND_RENDERER[] = { "OpenGL", "Vulkan" };
static const char *LIST_RENDERER_ACCURACY[] = { "Standard", "High" };

GpuSettingsWidget::GpuSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent) :
    BaseSettingsWidget(gui, emuenv, parent),
    ui(new Ui::GpuSettingsWidget) {
    ui->setupUi(this);

    initialize_settings();
}

void GpuSettingsWidget::init_ui() {
    //TODO: add opengl to renderer when renderer is ready
    emuenv.cfg.backend_renderer = LIST_BACKEND_RENDERER[1];
    ui->renderer->addItem(LIST_BACKEND_RENDERER[1]);

    //TODO: add gpu_list to settings when renderer is ready
    /*const std::vector<std::string> gpu_list_str = emuenv.renderer->get_gpu_list();
    std::vector<const char *> gpu_list;
    for (const auto &gpu : gpu_list_str)
        gpu_list.push_back(gpu.c_str());*/

    for (const auto accuracy : LIST_RENDERER_ACCURACY)
        ui->accuracy->addItem(accuracy);
    ui->accuracy->setCurrentIndex(cfg.high_accuracy);

    //TODO: init disable_surface_sync when renderer is ready
    //TODO: init screen_filters when renderer is ready

    ui->resolution_upscaler->setValue(cfg.resolution_multiplier);
    ui->resolution_multiplier_label->setText(QString("%1x").arg(cfg.resolution_multiplier));
    ui->resolution_label->setText(QString("%1x%2").arg(960 * cfg.resolution_multiplier).arg(544 * cfg.resolution_multiplier));

    ui->anisotropic_filtering->setValue(cfg.anisotropic_filtering);
    ui->anisotropic_filtering_label->setText(QString("%1x").arg(cfg.anisotropic_filtering));

    ui->use_shader_cache->setChecked(emuenv.cfg.shader_cache);
}

void GpuSettingsWidget::init_events() {
    connect(ui->accuracy, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        cfg.high_accuracy = static_cast<bool>(index);
    });

    connect(ui->resolution_upscaler, &QSlider::valueChanged, this, [this](int value) {
        ui->resolution_multiplier_label->setText(QString("%1x").arg(value));
        ui->resolution_label->setText(QString("%1x%2").arg(960 * value).arg(544 * value));
        cfg.resolution_multiplier = value;
    });

    connect(ui->anisotropic_filtering, &QSlider::valueChanged, this, [this](int value) {
        ui->anisotropic_filtering_label->setText(QString("%1x").arg(value));
        cfg.anisotropic_filtering = value;
    });

    connect(ui->use_shader_cache, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.shader_cache = ui->use_shader_cache->isChecked();
    });
}

GpuSettingsWidget::~GpuSettingsWidget() {
    delete ui;
}
