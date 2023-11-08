#include "emulator_settings_widget.h"
#include "ui_emulator_settings_widget.h"

static const char *LIST_BACKEND_AUDIO[] = { "SDL", "Cubeb" };
static const char *LIST_LOG_LEVEL[] = { "Trace", "Debug", "Info", "Warning", "Error", "Critical", "Off" };

EmulatorSettingsWidget::EmulatorSettingsWidget(GuiState &gui, EmuEnvState &emuenv, QWidget *parent) :
    BaseSettingsWidget(gui, emuenv, parent),
    ui(new Ui::EmulatorSettingsWidget) {
    ui->setupUi(this);

    initialize_settings();
}

void EmulatorSettingsWidget::init_ui() {
    ui->fullscreen->setChecked(emuenv.cfg.boot_apps_full_screen);

    auto current_audio_backend = 0;
    for (const auto backend : LIST_BACKEND_AUDIO) {
        ui->audio_backend->addItem(backend);
        if (emuenv.cfg.audio_backend == backend)
            ui->audio_backend->setCurrentIndex(current_audio_backend);
        current_audio_backend++;
    }

    ui->enable_ngs->setChecked(cfg.ngs_enable);

    auto current_log_level = 0;
    for (const auto level : LIST_LOG_LEVEL) {
        ui->log_level->addItem(level);
        if (emuenv.cfg.log_level == current_log_level)
            ui->log_level->setCurrentIndex(current_log_level);
        current_log_level++;
    }

    ui->archieve_log->setChecked(emuenv.cfg.archive_log);
    ui->discord_rte->setChecked(emuenv.cfg.discord_rich_presence);
    ui->texture_cache->setChecked(emuenv.cfg.texture_cache);
    ui->show_compile_shaders->setChecked(emuenv.cfg.show_compile_shaders);
    ui->show_touchpad_cursor->setChecked(cfg.show_touchpad_cursor);
    ui->log_compatibility_warnings->setChecked(emuenv.cfg.log_compat_warn);
    //TODO: add other settings for perf overlay
    ui->performance_overlay->setChecked(emuenv.cfg.performance_overlay);

    ui->current_emu_folder->setText(QString("\nCurrent emulator folder: %1").arg(emuenv.cfg.pref_path.c_str()));
    //TODO: make reset/change emu folder buttons functional
    //TODO: make clear custom config button functional
}

void EmulatorSettingsWidget::init_events() {
    connect(ui->fullscreen, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.boot_apps_full_screen = ui->fullscreen->isChecked();
    });

    connect(ui->audio_backend, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        emuenv.cfg.audio_backend = LIST_BACKEND_AUDIO[index];
    });

    connect(ui->log_level, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        emuenv.cfg.log_level = index;
        logging::set_level(static_cast<spdlog::level::level_enum>(emuenv.cfg.log_level));
    });

    connect(ui->archieve_log, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.archive_log = ui->archieve_log->isChecked();
    });

    connect(ui->discord_rte, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.discord_rich_presence = ui->discord_rte->isChecked();
    });

    connect(ui->texture_cache, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.texture_cache = ui->texture_cache->isChecked();
    });

    connect(ui->show_compile_shaders, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.show_compile_shaders = ui->show_compile_shaders->isChecked();
    });

    connect(ui->show_touchpad_cursor, &QCheckBox::stateChanged, this, [this]() {
        cfg.show_touchpad_cursor = ui->show_touchpad_cursor->isChecked();
    });

    connect(ui->log_compatibility_warnings, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.log_compat_warn = ui->log_compatibility_warnings->isChecked();
    });

    connect(ui->performance_overlay, &QCheckBox::stateChanged, this, [this]() {
        emuenv.cfg.performance_overlay = ui->performance_overlay->isChecked();
    });
}

EmulatorSettingsWidget::~EmulatorSettingsWidget() {
    delete ui;
}
