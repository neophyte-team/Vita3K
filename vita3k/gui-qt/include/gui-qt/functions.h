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

#pragma once

#include <gui/state.h>
#include <config/state.h>

namespace gui_qt {
void error(const std::string &message);

void pre_init(GuiState &gui, EmuEnvState &emuenv);
void init(GuiState &gui, EmuEnvState &emuenv);
void init_user_app(GuiState &gui, EmuEnvState &emuenv, const std::string &app_path);
void init_home(GuiState &gui, EmuEnvState &emuenv);
void draw_initial_setup(GuiState &gui, EmuEnvState &emuenv);

User create_temp_user(GuiState &gui, EmuEnvState &emuenv);
void get_users_list(GuiState &gui, EmuEnvState &emuenv);
void save_user(GuiState &gui, EmuEnvState &emuenv, const User &user);
void delete_user(GuiState &gui, EmuEnvState &emuenv, const User &user);
void select_user(GuiState &gui, EmuEnvState &emuenv, const User &user);
void init_user(GuiState &gui, EmuEnvState &emuenv, const User &user);

void save_config(GuiState &gui, EmuEnvState &emuenv, Config::CurrentConfig config);

void change_emulator_path(GuiState &gui, EmuEnvState &emuenv);
void reset_emulator(GuiState &gui, EmuEnvState &emuenv);
}