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
        auto constexpr a = SDLK_a;
        auto constexpr d = SDLK_d;
        auto constexpr w = SDLK_w;
        auto constexpr left = SDLK_LEFT;
        auto constexpr right = SDLK_RIGHT;
        auto constexpr up = SDLK_UP;
        auto constexpr space = SDLK_SPACE;
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

using Window   = SDL_Window;
using Renderer = SDL_Renderer;
using Texture  = SDL_Texture;

using Rect  = SDL_Rect;
using Event = SDL_Event;
using Optional_event = std::optional<Event>;

using Unique_window   = std::unique_ptr<Window, Window_deleter>;
using Unique_renderer = std::unique_ptr<Renderer, Renderer_deleter>;
using Unique_texture  = std::unique_ptr<Texture, Texture_deleter>;

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

Unique_window create_window(std::string const& title, int width, int height);
Unique_renderer create_renderer(Window& window);

void render_clear(Renderer& renderer);
void render_present(Renderer& renderer);

enum class Flip {
        none = SDL_FLIP_NONE,
        vertical = SDL_FLIP_VERTICAL,
        horizontal = SDL_FLIP_HORIZONTAL
};

void render_copy(Renderer& renderer, 
                 Texture& texture, 
                 Rect source, 
                 Rect destination,
                 double angle=0,
                 Flip flip=Flip::none);

void render_copy(Renderer& renderer,
                 Texture& texture,
                 Complex_number position,
                 double angle=0,
                 Flip flip=Flip::none);

Unique_texture load_texture(Renderer& renderer, std::string path);

bool has_intersection(Rect r1, Rect r2) noexcept;

struct Dimensions {
        int width;
        int height;
};

Dimensions texture_dimensions(Texture& texture);
int texture_width(Texture& texture);
int texture_height(Texture& texture);

Duration::Milliseconds get_ticks() noexcept;

Optional_event poll_event();

}

