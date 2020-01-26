/* BSD 3-Clause License
 *
 * Copyright © 2008-2020, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include "display.h"

#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

#include <SDL.h>
#include "globals.h"
#include "../console.h"
#include "../console.hpp"
#include "../libtcod_int.h"
#include "../tileset/fallback.h"
#include "../renderer_sdl2.h"
static struct TCOD_Tileset* ensure_tileset()
{
  if (!TCOD_ctx.tileset) {
    TCOD_ctx.tileset = TCOD_tileset_load_fallback_font_(0, 12);
  }
  if (!TCOD_ctx.tileset) {
    // "Couldn't load a fallback font for the SDL2/OPENGL2 renderer: "
    return NULL;
  }
  return TCOD_ctx.tileset;
}
namespace tcod {
/**
 *  Return an environment value as a std::string.
 *
 *  The returned string will be empty if the environment value does not exist.
 */
static std::string get_env(const char* key)
{
  char* value = std::getenv(key);
  return value ? value : "";
}
/**
 *  Set `renderer` from the TCOD_RENDERER environment variable if it exists.
 */
static void get_env_renderer(TCOD_renderer_t& renderer)
{
  const std::string value(get_env("TCOD_RENDERER"));
  if (value == "sdl") {
    renderer = TCOD_RENDERER_SDL;
  } else if (value == "opengl") {
    renderer = TCOD_RENDERER_OPENGL;
  } else if (value == "glsl") {
    renderer = TCOD_RENDERER_GLSL;
  } else if (value == "sdl2") {
    renderer = TCOD_RENDERER_SDL2;
  } else if (value == "opengl2") {
    renderer = TCOD_RENDERER_OPENGL2;
  }
}
/**
 *  Set `vsync` from the TCOD_VSYNC environment variable if it exists.
 */
static void get_env_vsync(bool& vsync)
{
  const std::string value(get_env("TCOD_RENDERER"));
  if (value == "0") {
    vsync = 0;
  } else if (value == "1") {
    vsync = 1;
  }
}
namespace console {
void init_root(int w, int h, const std::string& title, bool fullscreen,
               TCOD_renderer_t renderer, bool vsync)
{
  check_throw_error(
    TCOD_console_init_root_(w, h, title.c_str(), fullscreen, renderer, vsync)
  );
}
void init_root(int w, int h, const std::string& title, bool fullscreen,
               TCOD_renderer_t renderer)
{
  check_throw_error(
    TCOD_console_init_root(w, h, title.c_str(), fullscreen, renderer)
  );
}
} // namespace console
} // namespace tcod
TCOD_Error TCOD_console_init_root_(
    int w,
    int h,
    const char* title,
    bool fullscreen,
    TCOD_renderer_t renderer,
    bool vsync)
{
  if (w <= 0 || h <= 0) {
    TCOD_set_errorvf("Width and height must be greater than zero. Not %i,%i",
                     w, h);
    return TCOD_E_INVALID_ARGUMENT;
  }
  tcod::get_env_renderer(renderer);
  tcod::get_env_vsync(vsync);
  TCOD_console_delete(NULL);
  TCODConsole::root->data = TCOD_ctx.root = TCOD_console_new(w, h);
#ifndef TCOD_BARE
  TCOD_ctx.renderer=renderer;
#endif
  if (title) {
    strncpy(TCOD_ctx.window_title, title, sizeof(TCOD_ctx.window_title) - 1);
  }
  TCOD_ctx.fullscreen = fullscreen;
  switch (renderer) {
    case TCOD_RENDERER_OPENGL2:
    case TCOD_RENDERER_SDL2: {
      int renderer_flags = SDL_RENDERER_PRESENTVSYNC * vsync;
      int window_flags = (SDL_WINDOW_RESIZABLE |
                          (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
      struct TCOD_Tileset* tileset = ensure_tileset();
      TCOD_ctx.engine = TCOD_renderer_init_sdl2(
          w * tileset->tile_width, h * tileset->tile_height,
          title, window_flags, renderer_flags, tileset);
      break;
    }
    default:
      if(!TCOD_console_init(TCOD_ctx.root, title, fullscreen)) {
        return TCOD_E_ERROR;
      }
      break;
  }
  return TCOD_E_OK;
}
TCOD_Error TCOD_console_init_root(int w, int h, const char* title,
                                  bool fullscreen, TCOD_renderer_t renderer)
{
  return TCOD_console_init_root_(w, h, title, fullscreen, renderer, false);
}
void TCOD_quit(void)
{
  TCOD_console_delete(NULL);
}
void TCOD_console_set_window_title(const char *title)
{
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  SDL_SetWindowTitle(window, title);
  strncpy(TCOD_ctx.window_title, title, sizeof(TCOD_ctx.window_title) - 1);
}
void TCOD_console_set_fullscreen(bool fullscreen)
{
  TCOD_ctx.fullscreen = fullscreen;
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (window) {
    SDL_SetWindowFullscreen(
        window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  } else { // Deprecated renderer.
    TCOD_IFNOT(TCOD_ctx.root != NULL) { return; }
    TCOD_sys_set_fullscreen(fullscreen);
  }
}
bool TCOD_console_is_fullscreen(void)
{
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) { return TCOD_ctx.fullscreen; }
  return (SDL_GetWindowFlags(window)
          & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0;
}
bool TCOD_console_has_mouse_focus(void)
{
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) { return TCOD_ctx.app_has_mouse_focus; }
  return (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS) != 0;
}
bool TCOD_console_is_active(void)
{
  struct SDL_Window* window = TCOD_sys_get_sdl_window();
  if (!window) { return TCOD_ctx.app_is_active; }
  return (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS) != 0;
}
bool TCOD_console_is_window_closed(void) {
	return TCOD_ctx.is_window_closed;
}
struct SDL_Window* TCOD_sys_get_sdl_window(void)
{
  if (TCOD_ctx.engine && TCOD_ctx.engine->get_sdl_window) {
    return TCOD_ctx.engine->get_sdl_window(TCOD_ctx.engine);
  }
  return TCOD_sys_get_sdl_window_();
}
struct SDL_Renderer* TCOD_sys_get_sdl_renderer(void)
{
  if (TCOD_ctx.engine && TCOD_ctx.engine->get_sdl_renderer) {
    return TCOD_ctx.engine->get_sdl_renderer(TCOD_ctx.engine);
  }
  return TCOD_sys_get_sdl_renderer_();
}
int TCOD_sys_accumulate_console(const TCOD_Console* console)
{
  return TCOD_sys_accumulate_console_(console, nullptr);
}
int TCOD_sys_accumulate_console_(const TCOD_Console* console, const struct SDL_Rect* viewport)
{
  console = tcod::console::validate_(console);
  if (!console) { return -1; }
  if (TCOD_ctx.engine && TCOD_ctx.engine->accumulate) {
    return TCOD_ctx.engine->accumulate(TCOD_ctx.engine, console, viewport);
  }
  return -1;
}
