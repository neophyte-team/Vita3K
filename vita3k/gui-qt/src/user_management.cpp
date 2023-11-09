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

#include <config/functions.h>
#include <config/state.h>

#include <display/state.h>
#include <emuenv/state.h>
#include <gui/functions.h>
#include <host/dialog/filesystem.hpp>
#include <io/state.h>
#include <np/common.h>

#include <util/log.h>
#include <util/string_utils.h>

#include "gui-qt/functions.h"
#include <pugixml.hpp>

namespace gui_qt {

static std::string user_id_selected;

static auto get_users_index(GuiState &gui, const std::string &user_name) {
    const auto user_index = std::find_if(gui.users.begin(), gui.users.end(), [&](const auto &u) {
        return u.second.name == user_name;
    });

    return user_index;
}

User create_temp_user(GuiState &gui, EmuEnvState &emuenv) {
    auto id = 0;
    for (const auto &user : gui.users) {
        if (id != string_utils::stoi_def(user.first, 0, "gui user id"))
            break;
        else
            ++id;
    }
    user_id_selected = fmt::format("{:0>2d}", id);
    auto i = 1;
    const auto user = gui.lang.user_management["user"].c_str();
    for (; i < gui.users.size(); i++) {
        if (get_users_index(gui, user + std::to_string(i)) == gui.users.end())
            break;
    }

    User temp;
    temp.id = user_id_selected;
    temp.name = user + std::to_string(id);
    temp.avatar = "default";
    temp.theme_id = "default";
    temp.use_theme_bg = true;
    temp.start_type = "default";

    return temp;
}

void get_users_list(GuiState &gui, EmuEnvState &emuenv) {
    gui.users.clear();
    const auto user_path{ emuenv.pref_path / "ux0/user" };
    if (fs::exists(user_path) && !fs::is_empty(user_path)) {
        for (const auto &path : fs::directory_iterator(user_path)) {
            pugi::xml_document user_xml;
            if (fs::is_directory(path) && user_xml.load_file(((path / "user.xml").c_str()))) {
                const auto user_child = user_xml.child("user");

                // Load user id
                std::string user_id;
                if (!user_child.attribute("id").empty())
                    user_id = user_child.attribute("id").as_string();
                else
                    user_id = path.path().stem().string();

                // Load user name
                auto &user = gui.users[user_id];
                user.id = user_id;
                if (!user_child.attribute("name").empty())
                    user.name = user_child.attribute("name").as_string();
            }
        }
    }
}

void save_user(GuiState &gui, EmuEnvState &emuenv, const User &user) {
    const auto user_path{ emuenv.pref_path / "ux0/user" / user.id };
    if (!fs::exists(user_path))
        fs::create_directories(user_path);

    pugi::xml_document user_xml;
    auto declarationUser = user_xml.append_child(pugi::node_declaration);
    declarationUser.append_attribute("version") = "1.0";
    declarationUser.append_attribute("encoding") = "utf-8";

    // Save user settings
    auto user_child = user_xml.append_child("user");
    user_child.append_attribute("id") = user.id.c_str();
    user_child.append_attribute("name") = user.name.c_str();
    user_child.append_child("avatar").append_child(pugi::node_pcdata).set_value(user.avatar.c_str());

    // Save sort Apps list settings
    auto sort_apps_list = user_child.append_child("sort-apps-list");
    sort_apps_list.append_attribute("type") = user.sort_apps_type;
    sort_apps_list.append_attribute("state") = user.sort_apps_state;

    // Save theme settings
    auto theme = user_child.append_child("theme");
    theme.append_attribute("use-background") = user.use_theme_bg;
    theme.append_child("content-id").append_child(pugi::node_pcdata).set_value(user.theme_id.c_str());

    // Save start screen settings
    auto start_screen = user_child.append_child("start-screen");
    start_screen.append_attribute("type") = user.start_type.c_str();
    start_screen.append_child("path").append_child(pugi::node_pcdata).set_value(user.start_path.c_str());

    // Save backgrounds path
    auto bg_path = user_child.append_child("backgrounds");
    for (const auto &bg : user.backgrounds)
        bg_path.append_child("background").append_child(pugi::node_pcdata).set_value(bg.c_str());

    const auto save_xml = user_xml.save_file((user_path / "user.xml").c_str());
    if (!save_xml)
        LOG_ERROR("Fail save xml for user id: {}, name: {}, in path: {}", user.id, user.name, user_path.string());
}

void delete_user(GuiState &gui, EmuEnvState &emuenv, const User &user) {
    const auto user_path{ emuenv.pref_path / "ux0/user" };
    fs::remove_all(user_path / user.id);
    gui.users.erase(get_users_index(gui, gui.users[user.id].name));
    if (gui.users.empty() || (user_id_selected == emuenv.io.user_id)) {
        emuenv.cfg.user_id.clear();
        config::serialize_config(emuenv.cfg, emuenv.cfg.config_path);
        emuenv.io.user_id.clear();
    }
}

void select_user(GuiState &gui, EmuEnvState &emuenv, const User &user) {
    if (emuenv.io.user_id != user.id)
        gui_qt::init_user(gui, emuenv, user);
    if (emuenv.cfg.user_id != user.id) {
        emuenv.cfg.user_id = user.id;
        config::serialize_config(emuenv.cfg, emuenv.cfg.config_path);
    }
}

void init_user(GuiState &gui, EmuEnvState &emuenv, const User &user) {
    emuenv.io.user_id = user.id;
    emuenv.io.user_name = gui.users[user.id].name;
    gui::init_notice_info(gui, emuenv);
    gui::init_last_time_apps(gui, emuenv);
}

} // namespace gui_qt
