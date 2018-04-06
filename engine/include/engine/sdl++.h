#pragma once

#include "base.h"
#include <SDL2/SDL.h>
#include <memory>
#include <utility>
#include <string>
#include <complex>
#include <exception>
#include <optional>
#include <type_traits>

namespace Engine::Sdl {

using Keycode = SDL_Keycode;

namespace Keycodes {
        auto constexpr e = SDLK_e;
        auto constexpr a = SDLK_a;
        auto constexpr d = SDLK_d;
        auto constexpr w = SDLK_w;
        auto constexpr left = SDLK_LEFT;
        auto constexpr right = SDLK_RIGHT;
        auto constexpr up = SDLK_UP;
        auto constexpr space = SDLK_SPACE;
        auto constexpr left_shift = SDLK_LSHIFT;
}

using Window   = SDL_Window;
using Renderer = SDL_Renderer;
using Texture  = SDL_Texture;

struct Window_deleter {
        void operator()(Window* window) const noexcept;
};

struct Renderer_deleter {
        void operator()(Renderer* renderer) const noexcept;
};

struct Texture_deleter {
        void operator()(Texture* texture) const noexcept;
};

using Unique_window   = std::unique_ptr<Window, Window_deleter>;
using Unique_renderer = std::unique_ptr<Renderer, Renderer_deleter>;
using Unique_texture  = std::unique_ptr<Texture, Texture_deleter>;

using Rect  = SDL_Rect;
using Event = SDL_Event;
using Optional_event = std::optional<Event>;

class Error : public std::exception {
public:
        char const* what() const noexcept override;
};

class Manager {
public:
        Manager();
        ~Manager();

        Manager(Manager const&) = delete;
        Manager(Manager&&) = delete;
        
        Manager& operator=(Manager const&) = delete;
        Manager& operator=(Manager&&) = delete;
};

class Renderer_color_lock {
public:
        Renderer_color_lock(Renderer& renderer, Color color);

        Renderer_color_lock(Renderer_color_lock const&) = delete;
        Renderer_color_lock(Renderer_color_lock&&) = delete;

        Renderer_color_lock& operator=(Renderer_color_lock const&) = delete;
        Renderer_color_lock& operator=(Renderer_color_lock&&) = delete;

        ~Renderer_color_lock();

private:
        Renderer& renderer_;
        Color previous_color_;
};

Unique_window create_window(std::string const& title, int width, int height);
Unique_renderer create_renderer(Window& window, Color color=Color::white());

void set_render_color(Renderer& renderer, Color color);
Color get_render_color(Renderer& renderer);

void render_clear(Renderer& renderer);
void render_present(Renderer& renderer);

enum class Flip {
        none = SDL_FLIP_NONE,
        vertical = SDL_FLIP_VERTICAL,
        horizontal = SDL_FLIP_HORIZONTAL
};

void render_rect(Renderer& renderer, Rect rect, Color color);
void render_copy(Renderer& renderer, Texture& texture, Rect source, Rect destination);
void render_copy(Renderer& renderer,
                 Texture& texture,
                 Rect source,
                 Rect destination,
                 double angle=0,
                 Flip flip=Flip::none);

Unique_texture load_texture(Renderer& renderer, std::string path);

Rect make_rect(Complex_number position, Dimensions dimensions) noexcept;
Dimensions rect_dimensions(Rect rect) noexcept;

Dimensions texture_dimensions(Texture& texture);
Dimensions renderer_dimensions(Sdl::Renderer& renderer);

bool has_intersection(Rect r1, Rect r2) noexcept;
Duration::Milliseconds get_ticks() noexcept;
Optional_event poll_event();

}

