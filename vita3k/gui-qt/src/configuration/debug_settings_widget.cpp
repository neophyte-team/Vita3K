#include "debug_settings_widget.h"
#include "ui_debug_settings_widget.h"
#include <kernel/state.h>

DebugSettingsWidget::DebugSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent) :
    BaseSettingsWidget(gui, emuenv, parent),
    ui(new Ui::DebugSettingsWidget) {
    ui->setupUi(this);

    initialize_settings();
}

void DebugSettingsWidget::init_ui() {
    ui->import_logging->setChecked(emuenv.kernel.debugger.log_imports);
    ui->export_logging->setChecked(emuenv.kernel.debugger.log_exports);
    ui->shader_logging->setChecked(emuenv.cfg.log_active_shaders);
    ui->uniform_logging->setChecked(emuenv.cfg.log_uniforms);
    ui->save_color_surfaces->setChecked(emuenv.cfg.color_surface_debug);
    ui->elf_dumping->setChecked(emuenv.kernel.debugger.dump_elfs);
    ui->val_layers->setChecked(emuenv.cfg.validation_layer);

    //TODO: add the same for Texture Dumping, Watch Code, Watch Import Calls
}

void DebugSettingsWidget::init_events() {
    connect(ui->import_logging, &QCheckBox::stateChanged, this, [this]() {
        emuenv.kernel.debugger.log_imports = ui->import_logging->isChecked();
    });

    connect(ui->export_logging, &QCheckBox::stateChanged, this, [this]() {
        emuenv.kernel.debugger.log_exports = ui->export_logging->isChecked();
    });

    connect(ui->shader_logging, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.log_active_shaders = ui->shader_logging->isChecked();
    });

    connect(ui->uniform_logging, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.log_uniforms = ui->uniform_logging->isChecked();
    });

    connect(ui->save_color_surfaces, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.color_surface_debug = ui->save_color_surfaces->isChecked();
    });

    connect(ui->elf_dumping, &QCheckBox::stateChanged, this, [this]() {
        emuenv.kernel.debugger.dump_elfs = ui->elf_dumping->isChecked();
    });

    connect(ui->val_layers, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.validation_layer = ui->val_layers->isChecked();
    });
}

DebugSettingsWidget::~DebugSettingsWidget() {
    delete ui;
}
