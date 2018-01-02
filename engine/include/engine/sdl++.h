#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <utility>
#include <string>
#include <complex>
#include <exception>
#include <optional>
#include <type_traits>

// TODO Don't take `unique_ptr`s in interfaces

namespace Engine::Sdl {

using Keycode = SDL_Keycode;

namespace Keycodes {
        auto constexpr a = SDLK_a;
        auto constexpr d = SDLK_d;
        auto constexpr w = SDLK_w;
        auto constexpr space = SDLK_SPACE;
        auto constexpr left = SDLK_LEFT;
        auto constexpr right = SDLK_RIGHT;
};

class Window_deleter {
public:
        void operator()(SDL_Window* window) const noexcept;
};

class Renderer_deleter {
public:
        void operator()(SDL_Renderer* renderer) const noexcept;
};

class Texture_deleter {
public:
        void operator()(SDL_Texture* texture) const noexcept;
};

using Rect  = SDL_Rect;
using Color = SDL_Color;
using Event = SDL_Event;
using Optional_event = std::optional<Event>;

namespace Colors {

Color constexpr white {255, 255, 255, 255};

}

using Window   = std::unique_ptr<SDL_Window, Window_deleter>;
using Renderer = std::unique_ptr<SDL_Renderer, Renderer_deleter>;
using Texture  = std::unique_ptr<SDL_Texture, Texture_deleter>;

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

using WindowAndRenderer = std::pair<Window, Renderer>;

WindowAndRenderer create_window_and_renderer(std::string const& title,
                                             int width,
                                             int height,
                                             Color background_color=Colors::white);

void render_clear(Renderer const& renderer);
void render_present(Renderer const& renderer);

enum class Flip {
        none = SDL_FLIP_NONE,
        vertical = SDL_FLIP_VERTICAL,
        horizontal = SDL_FLIP_HORIZONTAL
};

void render_copy(Renderer const& renderer, 
                 Texture const& texture, 
                 Rect source, 
                 Rect destination,
                 double angle=0,
                 Flip flip=Flip::none);

Texture load_texture_from_file(Renderer const& renderer, std::string const& path);

struct Dimensions {
        int width;
        int height;
};

Dimensions texture_dimensions(Texture const& texture);

int get_ticks() noexcept;

Optional_event poll_event();

}

