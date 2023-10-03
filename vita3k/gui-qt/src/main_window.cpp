#include "main_window.h"
#include "QHBoxLayout"
#include "QLabel"
#include "QPushButton"
#include "gui/functions.h"
#include <host/dialog/filesystem.hpp>
#include <io/state.h>
#include <gui/state.h>
#include <packages/pkg.h>
#include <packages/sfo.h>
#include "initial_setup_wizard.h"
#include "interface.h"
#include "packages/pkg.h"
#include "rif2zrif.h"
#include "ui_main_window.h"
#include "util/string_utils.h"

static std::filesystem::path pkg_path = "";
static std::filesystem::path license_path = "";
static std::string zRIF;

MainWindow::MainWindow(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gui{gui_},
    emuenv{emuenv_} {
    ui->setupUi(this);
}

void MainWindow::init_live_area() {
    live_area = new LiveArea(gui, emuenv, this);
    live_area->initialize();

    setCentralWidget(live_area);

    connect(live_area, &LiveArea::selection_changed, this, &MainWindow::on_app_selection_changed, Qt::QueuedConnection);
}
void MainWindow::on_initial_setup() {
    auto *initial_setup_wizard = new InitialSetupWizard(gui, emuenv);
    initial_setup_wizard->show();
    this->close();
}

void MainWindow::on_app_selection_changed() {
    const auto selected_app = live_area->get_selected_app();
    if (!selected_app)
        return;

    LOG_ERROR("{}", selected_app->title);

    render_window = new RenderWindow(gui, emuenv, this);
    render_window->show();
    render_window->setFocus();

    render_window->init_render_target();

    /*gui::pre_run_app(gui, emuenv, selected_app->path);

    gui::set_config(gui, emuenv, emuenv.io.app_path);

    emuenv.app_info.app_version = selected_app->app_ver;
    emuenv.app_info.app_category = selected_app->category;
    emuenv.app_info.app_content_id = selected_app->content_id;
    emuenv.io.addcont = selected_app->addcont;
    emuenv.io.savedata = selected_app->savedata;
    emuenv.current_app_title = selected_app->title;
    emuenv.app_info.app_short_title = selected_app->stitle;
    emuenv.io.title_id = selected_app->title_id;
    int32_t main_module_id;

    if (!app::late_init(emuenv))
        LOG_ERROR("ERROR");

    {
        const auto err = load_app(main_module_id, emuenv, string_utils::utf_to_wide(emuenv.io.app_path));
    }*/
}

void MainWindow::on_actionInstall_pkg_triggered() {
    host::dialog::filesystem::Result result = host::dialog::filesystem::Result::CANCEL;
    result = host::dialog::filesystem::open_file(pkg_path, { { "PlayStation Store Downloaded Package", { "pkg" } } });

    if (result == host::dialog::filesystem::Result::SUCCESS) {
        auto *dialog = new QDialog(this);
        auto *box_layout = new QVBoxLayout(dialog);
        auto licence_button = new QPushButton(dialog);
        auto zrif_button = new QPushButton(dialog);
        licence_button->setText("Select work.bin/rif");
        zrif_button->setText("Enter zRIF");

        dialog->setModal(true);
        dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

        box_layout->addWidget(licence_button);
        box_layout->addWidget(zrif_button);
        //TODO: add installation via zrif

        connect(licence_button, &QPushButton::released, this, &MainWindow::on_licence_button);

        dialog->exec();
    }
}

void MainWindow::on_licence_button() {
    host::dialog::filesystem::Result result = host::dialog::filesystem::Result::CANCEL;
    result = host::dialog::filesystem::open_file(license_path, { { "PlayStation Vita software license file", { "bin", "rif" } } });
    if (result == host::dialog::filesystem::Result::SUCCESS) {
        fs::ifstream bin_file(license_path.wstring(), std::ios::in | std::ios::binary | std::ios::ate);
        zRIF = rif2zrif(bin_file);

        start_pkg_installation();
    }
}

void MainWindow::start_pkg_installation() {
    static std::atomic<float> progress(0);
    static std::mutex install_mutex;
    static const auto progress_callback = [&](float updated_progress) {
        progress = updated_progress;
    };

    std::lock_guard<std::mutex> lock(install_mutex);
    std::thread installation([this]() {
        if (install_pkg(pkg_path.string(), emuenv, zRIF, progress_callback)) {
            std::lock_guard<std::mutex> lock(install_mutex);

            //TODO: show progress dialog
            LOG_ERROR("INSTALLED");
            //TODO: refresh app list
        }
        zRIF.clear();
    });
    installation.detach();
}

MainWindow::~MainWindow() {
    delete ui;
}
