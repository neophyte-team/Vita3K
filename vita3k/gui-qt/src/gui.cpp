// Vita3K emulator project
// Copyright (C) 2023 Vita3K team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <gui-qt/functions.h>
#include <gui/functions.h>
#include <lang/functions.h>
#include <regmgr/functions.h>
#include <io/functions.h>
#include <config/functions.h>
#include <pugixml.hpp>

#include <QApplication>
#include <QVulkanInstance>
#include <QVulkanWindow>
#include <QWindow>

namespace gui_qt {

static bool get_user_apps(GuiState &gui, EmuEnvState &emuenv) {
    const auto apps_cache_path{ fs::path(emuenv.pref_path) / "ux0/temp/apps.dat" };
    fs::ifstream apps_cache(apps_cache_path, std::ios::in | std::ios::binary);
    if (apps_cache.is_open()) {
        gui.app_selector.user_apps.clear();
        // Read size of apps list
        size_t size;
        apps_cache.read((char *)&size, sizeof(size));

        // Check version of cache
        uint32_t versionInFile;
        apps_cache.read((char *)&versionInFile, sizeof(uint32_t));
        if (versionInFile != 1) {
            LOG_WARN("Current version of cache: {}, is outdated, recreate it.", versionInFile);
            return false;
        }

        // Read language of cache
        apps_cache.read((char *)&gui.app_selector.apps_cache_lang, sizeof(uint32_t));
        if (gui.app_selector.apps_cache_lang != emuenv.cfg.sys_lang) {
            LOG_WARN("Current lang of cache: {}, is diferent config: {}, recreate it.", gui::get_sys_lang_name(gui.app_selector.apps_cache_lang), gui::get_sys_lang_name(emuenv.cfg.sys_lang));
            return false;
        }

        // Read App info value
        for (size_t a = 0; a < size; a++) {
            auto read = [&apps_cache]() {
                size_t size;

                apps_cache.read((char *)&size, sizeof(size));

                std::vector<char> buffer(size); // dont trust std::string to hold buffer enough
                apps_cache.read(buffer.data(), size);

                return std::string(buffer.begin(), buffer.end());
            };

            gui::App app;

            app.app_ver = read();
            app.category = read();
            app.content_id = read();
            app.addcont = read();
            app.savedata = read();
            app.parental_level = read();
            app.stitle = read();
            app.title = read();
            app.title_id = read();
            app.path = read();

            gui.app_selector.user_apps.push_back(app);
        }

        init_apps_icon(gui, emuenv, gui.app_selector.user_apps);
        gui::load_and_update_compat_user_apps(gui, emuenv);
    }

    return !gui.app_selector.user_apps.empty();
}

void pre_init(GuiState &gui, EmuEnvState &emuenv) {
    lang::init_lang(gui.lang, emuenv);

}

void init(GuiState &gui, EmuEnvState &emuenv) {
    gui::get_modules_list(gui, emuenv);
    gui::get_notice_list(emuenv);
    gui_qt::get_users_list(gui, emuenv);
    gui::get_time_apps(gui, emuenv);

    if (emuenv.cfg.show_welcome)
        gui.help_menu.welcome_dialog = true;

    gui::get_sys_apps_title(gui, emuenv);

    gui_qt::init_home(gui, emuenv);

    // Initialize trophy callback
    emuenv.np.trophy_state.trophy_unlock_callback = [&gui](NpTrophyUnlockCallbackData &callback_data) {
        const std::lock_guard<std::mutex> guard(gui.trophy_unlock_display_requests_access_mutex);
        gui.trophy_unlock_display_requests.insert(gui.trophy_unlock_display_requests.begin(), callback_data);
    };
}

void init_user_app(GuiState &gui, EmuEnvState &emuenv, const std::string &app_path) {
    const auto APP_INDEX = gui::get_app_index(gui, app_path);
    if (APP_INDEX != gui.app_selector.user_apps.end()) {
        gui.app_selector.user_apps.erase(APP_INDEX);
        gui.app_selector.user_apps_icon.erase(app_path);
    }

    gui::get_app_param(gui, emuenv, app_path);

    const auto TIME_APP_INDEX = gui::get_time_app_index(gui, emuenv, app_path);
    if (TIME_APP_INDEX != gui.time_apps[emuenv.io.user_id].end())
        gui::get_app_index(gui, app_path)->last_time = TIME_APP_INDEX->last_time_used;
}

void init_home(GuiState &gui, EmuEnvState &emuenv) {
    if (gui.app_selector.user_apps.empty() && (emuenv.cfg.load_app_list || !emuenv.cfg.run_app_path)) {
        if (!get_user_apps(gui, emuenv))
            gui::init_user_apps(gui, emuenv);
    }

    regmgr::init_regmgr(emuenv.regmgr, emuenv.pref_path.wstring());

    if (!gui.users.empty() && gui.users.contains(emuenv.cfg.user_id) && emuenv.cfg.auto_user_login)
        gui_qt::init_user(gui, emuenv, gui.users[emuenv.cfg.user_id]);
}

void save_config(GuiState &gui, EmuEnvState &emuenv, Config::CurrentConfig config) {
    if (gui.configuration_menu.custom_settings_dialog) {
        const auto CONFIG_PATH{ emuenv.config_path / "config" };
        const auto CUSTOM_CONFIG_PATH{ CONFIG_PATH / fmt::format("config_{}.xml", emuenv.app_path) };
        if (!fs::exists(CONFIG_PATH))
            fs::create_directory(CONFIG_PATH);

        pugi::xml_document custom_config_xml;
        auto declarationUser = custom_config_xml.append_child(pugi::node_declaration);
        declarationUser.append_attribute("version") = "1.0";
        declarationUser.append_attribute("encoding") = "utf-8";

        // Config
        auto config_child = custom_config_xml.append_child("config");

        // Core
        auto core_child = config_child.append_child("core");
        core_child.append_attribute("modules-mode") = config.modules_mode;
        auto enable_module = core_child.append_child("lle-modules");
        for (const auto &m : config.lle_modules)
            enable_module.append_child("module").append_child(pugi::node_pcdata).set_value(m.c_str());

        // CPU
        auto cpu_child = config_child.append_child("cpu");
        cpu_child.append_attribute("cpu-backend") = config.cpu_backend.c_str();
        cpu_child.append_attribute("cpu-opt") = config.cpu_opt;

        // GPU
        auto gpu_child = config_child.append_child("gpu");
        gpu_child.append_attribute("high-accuracy") = config.high_accuracy;
        gpu_child.append_attribute("resolution-multiplier") = config.resolution_multiplier;
        gpu_child.append_attribute("disable-surface-sync") = config.disable_surface_sync;
        gpu_child.append_attribute("screen-filter") = config.screen_filter.c_str();
        gpu_child.append_attribute("v-sync") = config.v_sync;
        gpu_child.append_attribute("anisotropic-filtering") = config.anisotropic_filtering;
        gpu_child.append_attribute("import-textures") = config.import_textures;
        gpu_child.append_attribute("export-textures") = config.export_textures;
        gpu_child.append_attribute("export-as-png") = config.export_as_png;

        // System
        auto system_child = config_child.append_child("system");
        system_child.append_attribute("pstv-mode") = config.pstv_mode;

        // Emulator
        auto emulator_child = config_child.append_child("emulator");
        emulator_child.append_attribute("enable-ngs") = config.ngs_enable;
        emulator_child.append_attribute("show-touchpad-cursor") = config.show_touchpad_cursor;

        // Network
        auto network_child = config_child.append_child("network");
        network_child.append_attribute("psn-status") = config.psn_status;

        const auto save_xml = custom_config_xml.save_file(CUSTOM_CONFIG_PATH.c_str());
        if (!save_xml)
            LOG_ERROR("Failed to save custom config xml for app path: {}, in path: {}", emuenv.app_path, CONFIG_PATH.string());
    } else {
        emuenv.cfg.cpu_backend = config.cpu_backend;
        emuenv.cfg.cpu_opt = config.cpu_opt;
        emuenv.cfg.modules_mode = config.modules_mode;
        emuenv.cfg.lle_modules = config.lle_modules;
        emuenv.cfg.pstv_mode = config.pstv_mode;
        emuenv.cfg.high_accuracy = config.high_accuracy;
        emuenv.cfg.resolution_multiplier = config.resolution_multiplier;
        emuenv.cfg.disable_surface_sync = config.disable_surface_sync;
        emuenv.cfg.screen_filter = config.screen_filter;
        emuenv.cfg.v_sync = config.v_sync;
        emuenv.cfg.anisotropic_filtering = config.anisotropic_filtering;
        emuenv.cfg.import_textures = config.import_textures;
        emuenv.cfg.export_textures = config.export_textures;
        emuenv.cfg.export_as_png = config.export_as_png;
        emuenv.cfg.ngs_enable = config.ngs_enable;
        emuenv.cfg.show_touchpad_cursor = config.show_touchpad_cursor;
        emuenv.cfg.psn_status = config.psn_status;
    }

    config::serialize_config(emuenv.cfg, emuenv.cfg.config_path);
}

}