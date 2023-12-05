#include "main_window.h"
#include "interface.h"
#include "src/configuration/settings_window.h"
#include "src/initial-setup/initial_setup_wizard.h"
#include "src/common-dialogs/common_dialog.h"
#include "src/user_management_dialog.h"
#include "ui_main_window.h"
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QProgressBar>
#include <QProgressDialog>
#include <QPushButton>
#include <QThread>
#include <SDL_events.h>
#include <config/functions.h>
#include <config/version.h>
#include <ctrl/state.h>
#include <gui-qt/functions.h>
#include <gui/functions.h>
#include <gui/state.h>
#include <host/dialog/filesystem.hpp>
#include <io/state.h>
#include <packages/functions.h>
#include <packages/pkg.h>
#include <packages/sfo.h>
#include <rif2zrif.h>

#include <QFile>
#include <QTextStream>
#include <QDirIterator>

static std::filesystem::path pkg_path = "";
static std::filesystem::path license_path = "";
static std::filesystem::path archive_path = "";
static std::string zRIF;

static float global_progress = 0.f;
static float archives_count = 0.f;

static std::map<fs::path, std::vector<ContentInfo>> contents_archives;
static std::vector<fs::path> invalid_archives;

static int SDLEventWatcher(void *user_data, SDL_Event *event) {
    auto main_window = static_cast<MainWindow *>(user_data);

    //TODO: implement SDL even handling

    return 0;
}

MainWindow::MainWindow(GuiState &gui_, EmuEnvState &emuenv_, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gui{gui_},
    emuenv{emuenv_} {
    ui->setupUi(this);

    this->setWindowTitle(QString::fromStdString(window_title));

    SDL_AddEventWatch(&SDLEventWatcher, this);

    update_input_timer.setInterval(1);
    connect(&update_input_timer, &QTimer::timeout, this, &MainWindow::update_input);
    update_input_timer.start();

    refresh_controllers(emuenv.ctrl, emuenv);

    if (!emuenv.cfg.initial_setup) {
        on_initial_setup();
    } else {
        init_live_area();
    }
}

void MainWindow::update_input() {
    SDL_PumpEvents();

    handle_common_dialogs();
}

void MainWindow::handle_common_dialogs() {
    if (emuenv.common_dialog.status == SCE_COMMON_DIALOG_STATUS_RUNNING) {
        switch (emuenv.common_dialog.type) {
        case IME_DIALOG: {
            auto ime_dialog = new ImeDialog(gui, emuenv, this);
            ime_dialog->exec();
            break;
        }
        case MESSAGE_DIALOG: {
            auto message_dialog = new MessageDialog(gui, emuenv, this);
            message_dialog->exec();
            break;
        }
        case TROPHY_SETUP_DIALOG:
            //TODO: implement trophy dialog
            break;
        case SAVEDATA_DIALOG:
            //TODO: implement savedata dialog
            break;
        default:
            break;
        }
    }
}

void MainWindow::init_live_area() {
    gui_qt::init(gui, emuenv);
    this->show();

    live_area = new LiveArea(gui, emuenv, this);
    live_area->initialize();

    setCentralWidget(live_area);

    connect(live_area, &LiveArea::selection_changed, this, &MainWindow::on_app_selection_changed, Qt::QueuedConnection);

    if (!emuenv.cfg.auto_user_login) {
        auto user_management_dialog = new UserManagementDialog(gui, emuenv, this);
        user_management_dialog->exec();
    }
}
void MainWindow::on_initial_setup() {
    auto *initial_setup_wizard = new InitialSetupWizard(gui, emuenv, this);
    initial_setup_wizard->show();
    this->close();
}

void MainWindow::on_app_selection_changed() {
    const auto selected_app = live_area->get_selected_app();
    if (!selected_app)
        return;

    render_window = new RenderWindow(gui, emuenv, this);
    render_window->init_render_target();

    render_window->show();
    render_window->setFocus();
    live_area->hide();
    setCentralWidget(render_window);

    gui::set_config(gui, emuenv, selected_app->path);

    emuenv.app_info.app_version = selected_app->app_ver;
    emuenv.app_info.app_category = selected_app->category;
    emuenv.app_info.app_content_id = selected_app->content_id;
    emuenv.io.addcont = selected_app->addcont;
    emuenv.io.savedata = selected_app->savedata;
    emuenv.current_app_title = selected_app->title;
    emuenv.app_info.app_short_title = selected_app->stitle;
    emuenv.io.title_id = selected_app->title_id;
    emuenv.io.app_path = selected_app->path;

    if (!app::late_init(emuenv))
        LOG_ERROR("ERROR");

    int32_t main_module_id;
    {
        const auto err = load_app(main_module_id, emuenv, string_utils::utf_to_wide(selected_app->path));
    }

    {
        const auto err = run_app(emuenv, main_module_id);
    }
}

void MainWindow::on_actionInstall_Firmware_triggered() {
    static std::mutex install_mutex;
    std::filesystem::path pup_path = "";

    auto result = host::dialog::filesystem::open_file(pup_path, { { "PlayStation Vita Firmware Package", { "PUP" } } });

    if (result == host::dialog::filesystem::Result::SUCCESS) {
        auto progress_dialog = new QProgressDialog(this);
        progress_dialog->setWindowTitle("Installation");
        progress_dialog->setModal(true);
        progress_dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
        progress_dialog->setCancelButton(nullptr);

        std::lock_guard<std::mutex> lock(install_mutex);
        std::thread installation([this, &progress_dialog, &pup_path]() {
            auto progress_callback = [&](float updated_progress) {
                QMetaObject::invokeMethod(progress_dialog, "setValue", Qt::QueuedConnection, Q_ARG(int, static_cast<int>(updated_progress)));
            };
            install_pup(emuenv.pref_path.wstring(), pup_path.string(), progress_callback);
            std::lock_guard<std::mutex> lock(install_mutex);

            gui::get_firmware_version(emuenv);
            config::serialize_config(emuenv.cfg, emuenv.config_path);
        });
        installation.detach();
        progress_dialog->exec();
    }
}

void MainWindow::on_actionInstall_pkg_triggered() {
    host::dialog::filesystem::Result result = host::dialog::filesystem::Result::CANCEL;
    result = host::dialog::filesystem::open_file(pkg_path, { { "PlayStation Store Downloaded Package", { "pkg" } } });

    if (result == host::dialog::filesystem::Result::SUCCESS) {
        pkg_installation_dialog = new QDialog(this);
        auto *box_layout = new QVBoxLayout(pkg_installation_dialog);
        auto licence_button = new QPushButton(pkg_installation_dialog);
        auto zrif_button = new QPushButton(pkg_installation_dialog);
        licence_button->setText("Select work.bin/rif");
        zrif_button->setText("Enter zRIF");

        pkg_installation_dialog->setModal(true);
        pkg_installation_dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

        box_layout->addWidget(licence_button);
        box_layout->addWidget(zrif_button);

        connect(licence_button, &QPushButton::released, this, &MainWindow::on_licence_button);
        connect(zrif_button, &QPushButton::released, this, &MainWindow::on_zrif_button);

        pkg_installation_dialog->exec();
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

void MainWindow::on_zrif_button() {
    bool result;
    zRIF = QInputDialog::getText(this, tr("Enter zRIF key"),
        tr("Please input your zRIF here:"), QLineEdit::Normal,
        "", &result).toStdString();

    if (result && !zRIF.empty())
        start_pkg_installation();
}

void MainWindow::start_pkg_installation() {
    static std::mutex install_mutex;
    auto progress_dialog = new QProgressDialog(this);
    progress_dialog->setWindowTitle("Application Installation");
    progress_dialog->setModal(true);
    progress_dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress_dialog->setCancelButton(nullptr);

    std::lock_guard<std::mutex> lock(install_mutex);
    std::thread installation([this, &progress_dialog]() {
        auto progress_callback = [&](float updated_progress) {
            QMetaObject::invokeMethod(progress_dialog, "setValue", Qt::QueuedConnection, Q_ARG(int, static_cast<int>(updated_progress)));
        };
        if (install_pkg(pkg_path.string(), emuenv, zRIF, progress_callback)) {
            std::lock_guard<std::mutex> lock(install_mutex);

            if ((emuenv.app_info.app_category.find("gd") != std::string::npos) || (emuenv.app_info.app_category.find("gp") != std::string::npos)) {
                gui_qt::init_user_app(gui, emuenv, emuenv.app_info.app_title_id);
                gui::save_apps_cache(gui, emuenv);
            }

            live_area->refresh_app_list();
            pkg_installation_dialog->close();
        }
        zRIF.clear();
    });
    installation.detach();
    progress_dialog->exec();
}

void MainWindow::on_actionInstall_zip_vpk_triggered() {
    auto archive_installation_dialog = new QDialog(this);
    auto *box_layout = new QVBoxLayout(archive_installation_dialog);
    auto file_button = new QPushButton(archive_installation_dialog);
    auto directory_button = new QPushButton(archive_installation_dialog);
    file_button->setText("Select File");
    directory_button->setText("Select Directory");

    archive_installation_dialog->setModal(true);
    archive_installation_dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    box_layout->addWidget(file_button);
    box_layout->addWidget(directory_button);

    connect(file_button, &QPushButton::released, this, &MainWindow::on_file_button);
    connect(directory_button, &QPushButton::released, this, &MainWindow::on_directory_button);

    archive_installation_dialog->exec();
}

void MainWindow::on_file_button() {
    host::dialog::filesystem::Result result = host::dialog::filesystem::Result::CANCEL;
    // Set file filters for the file picking dialog
    std::vector<host::dialog::filesystem::FileFilter> file_filters = {
        { "PlayStation Vita commercial software package (NoNpDrm/FAGDec) / PlayStation Vita homebrew software package", { "zip", "vpk" } },
        { "PlayStation Vita commercial software package (NoNpDrm/FAGDec)", { "zip" } },
        { "PlayStation Vita homebrew software package", { "vpk" } },
    };
    // Call file picking dialog from the native file browser
    result = host::dialog::filesystem::open_file(archive_path, file_filters);

    static std::atomic<float> progress(0.f);

    static std::mutex install_mutex;
    auto progress_dialog = new QProgressDialog(this);
    progress_dialog->setWindowTitle("Archive Installation");
    progress_dialog->setModal(true);
    progress_dialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    progress_dialog->setCancelButton(nullptr);

    std::lock_guard<std::mutex> lock(install_mutex);
    std::thread installation([this, &progress_dialog]() {
        auto progress_callback = [&](ArchiveContents progress_value) {
            //TODO: fix progress callback
            //QMetaObject::invokeMethod(progress_dialog, "setValue", Qt::QueuedConnection, Q_ARG(int, static_cast<int>(progress_value.progress.value())));
        };
        global_progress = 1.f;
        const auto install_archive_contents = [&](const fs::path &archive_path) {
            const auto result = install_archive(emuenv, &gui, archive_path, progress_callback);
            std::lock_guard<std::mutex> lock(install_mutex);
            if (!result.empty()) {
                contents_archives[archive_path] = result;
                std::sort(contents_archives[archive_path].begin(), contents_archives[archive_path].end(), [&](const ContentInfo &lhs, const ContentInfo &rhs) {
                    return lhs.state < rhs.state;
                });
            } else
                invalid_archives.push_back(archive_path);
        };

        const auto contents_path = fs::path(archive_path.wstring());

        archives_count = global_progress = 1.f;
        install_archive_contents(contents_path);

        for (const auto &archive : contents_archives) {
            for (const auto &content : archive.second) {
                if (content.state) {
                    if (content.category == "gd") {
                        gui_qt::init_user_app(gui, emuenv, content.title_id);
                        gui::save_apps_cache(gui, emuenv);
                    }
                }
            }
        }

        progress = 0.f;
        archives_count = 0.f;
        global_progress = 0.f;
    });
    installation.detach();
    progress_dialog->exec();

    //TODO: maybe refactor file/directory installation
}

void MainWindow::on_directory_button() {

}

void MainWindow::on_actionSettings_triggered() {
    auto settings_window = new SettingsWindow(gui, emuenv);
    settings_window->show();
}

void MainWindow::on_actionUser_Management_triggered() {
    auto user_management_dialog = new UserManagementDialog(gui, emuenv, this);
    user_management_dialog->exec();
}

MainWindow::~MainWindow() {
    delete ui;
}
