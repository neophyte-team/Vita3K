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

#include <app/functions.h>

#include <audio/state.h>
#include <config/functions.h>
#include <config/state.h>
#include <config/version.h>
#include <display/state.h>
#include <emuenv/state.h>
#include <gui/imgui_impl_sdl.h>
#include <io/functions.h>
#include <kernel/state.h>
#include <ngs/state.h>
#include <renderer/state.h>

#include <nids/functions.h>
#include <renderer/functions.h>
#include <rtc/rtc.h>
#include <util/fs.h>
#include <util/lock_and_find.h>
#include <util/log.h>
#include <util/string_utils.h>

#if USE_DISCORD
#include <app/discord.h>
#endif

#include <gdbstub/functions.h>

#include <renderer/vulkan/functions.h>
#include <util/string_utils.h>

#include <SDL_video.h>
#include <SDL_vulkan.h>

namespace app {

bool init_qt(EmuEnvState &state, Config &cfg, const Root &root_paths) {
    state.cfg = std::move(cfg);

    state.base_path = root_paths.get_base_path_string();
    state.default_path = root_paths.get_pref_path_string();

    // If configuration does not provide a preference path, use SDL's default
    if (state.cfg.pref_path == root_paths.get_pref_path() || state.cfg.pref_path.empty())
        state.pref_path = string_utils::utf_to_wide(root_paths.get_pref_path_string());
    else {
        if (state.cfg.pref_path.back() != '/')
            state.cfg.pref_path += '/';
        state.pref_path = string_utils::utf_to_wide(state.cfg.pref_path);
    }

    state.backend_renderer = renderer::Backend::Vulkan;

    if (string_utils::toupper(state.cfg.backend_renderer) == "OPENGL") {
#ifndef __APPLE__
        state.backend_renderer = renderer::Backend::OpenGL;
#else
        state.cfg.backend_renderer = "Vulkan";
        config::serialize_config(state.cfg, state.cfg.config_path);
#endif
    }

    if (state.cfg.fullscreen) {
        state.display.fullscreen = true;
    }

    state.res_width_dpi_scale = static_cast<uint32_t>(DEFAULT_RES_WIDTH * state.dpi_scale);
    state.res_height_dpi_scale = static_cast<uint32_t>(DEFAULT_RES_HEIGHT * state.dpi_scale);

    // initialize the renderer first because we need to know if we need a page table
    /*if (!state.cfg.console) {
        if (renderer::init(state.window.get(), state.renderer, state.backend_renderer, state.cfg, state.base_path.data())) {
            update_viewport(state);
        } else {
            switch (state.backend_renderer) {
            case renderer::Backend::OpenGL:
                error_dialog("Could not create OpenGL context!\nDoes your GPU at least support OpenGL 4.4?", nullptr);
                break;

            case renderer::Backend::Vulkan:
                error_dialog("Could not create Vulkan context!\nDoes your device support Vulkan?");
                break;

            default:
                error_dialog(fmt::format("Unknown backend render: {}.", state.cfg.backend_renderer));
                break;
            }
            return false;
        }
    }*/

    if (!init(state.io, state.base_path, state.pref_path, state.cfg.console)) {
        LOG_ERROR("Failed to initialize file system for the emulator!");
        return false;
    }

    return true;
}

void error_dialog_qt(const std::string &message, QWindow *window) {

}

}