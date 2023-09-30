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
#include <config/state.h>

#include "regmgr/functions.h"
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
    gui::get_users_list(gui, emuenv);
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

void init_home(GuiState &gui, EmuEnvState &emuenv) {
    if (gui.app_selector.user_apps.empty() && (emuenv.cfg.load_app_list || !emuenv.cfg.run_app_path)) {
        if (!get_user_apps(gui, emuenv))
            gui::init_user_apps(gui, emuenv);
    }

    regmgr::init_regmgr(emuenv.regmgr, emuenv.pref_path.wstring());

    const auto is_cmd = emuenv.cfg.run_app_path || emuenv.cfg.content_path;
    if (!gui.users.empty() && gui.users.contains(emuenv.cfg.user_id) && (is_cmd || emuenv.cfg.auto_user_login)) {
        gui::init_user(gui, emuenv, emuenv.cfg.user_id);
        if (!is_cmd && emuenv.cfg.auto_user_login) {
            gui.vita_area.information_bar = true;
            gui::open_user(gui, emuenv);
        }
    } //else
        //gui::init_user_management(gui, emuenv);
}

}