#include "SDL_image.h"
#include "sdl++.h"

namespace Engine::Sdl {

void Window_deleter::operator()(Window* window) const noexcept
{
        SDL_DestroyWindow(window);
}

void Renderer_deleter::operator()(Renderer* renderer) const noexcept
{
        SDL_DestroyRenderer(renderer);
}

void Texture_deleter::operator()(Texture* texture) const noexcept
{
        SDL_DestroyTexture(texture);
}

char const* Error::what() const noexcept
{
        auto const* sdl_error = SDL_GetError();
        if (sdl_error)
                return sdl_error;

        auto const* img_error = IMG_GetError();
        if (img_error)
                return img_error;

        return "";
}

Manager::Manager()
{
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
                throw Error();
}

Manager::~Manager()
{
        SDL_Quit();
}

WindowAndRenderer create_window_and_renderer(
        std::string title, 
        int width, 
        int height,
        Color background_color)
{
        Window* window = nullptr;
        Renderer* renderer = nullptr;

        if (SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) < 0)
                throw Error();

        SDL_SetWindowTitle(window, title.c_str());
        
        if (SDL_SetRenderDrawColor(renderer, 
                                   background_color.r,
                                   background_color.g,
                                   background_color.b,
                                   background_color.a) < 0) {
                throw Error();
        }

        return WindowAndRenderer(Unique_window(window), Unique_renderer(renderer));
}

void render_clear(Renderer& renderer)
{
        if (SDL_RenderClear(&renderer) < 0)
                throw Error();
}

void render_present(Renderer& renderer)
{
        SDL_RenderPresent(&renderer);
}

void render_copy(Renderer& renderer, 
                 Texture& texture, 
                 Rect source, 
                 Rect destination,
                 double angle,
                 Flip flip)
{
        if (SDL_RenderCopyEx(&renderer,
                             &texture,
                             &source,
                             &destination,
                             angle,
                             nullptr, // Center point, fine this way
                             static_cast<SDL_RendererFlip>(flip)) < 0) {
                throw Error();
        }
}

void render_copy(Renderer& renderer,
                 Texture& texture,
                 Complex_number position,
                 double angle,
                 Flip flip)
{
        auto const [width, height] = texture_dimensions(texture);

        Rect const src {
                0,
                0,
                width,
                height
        };

        Rect const dst {
                static_cast<int>(position.real()),
                static_cast<int>(position.imag()),
                width,
                height
        };

        render_copy(renderer, texture, src, dst, angle, flip);
}

Unique_texture load_texture(Renderer& renderer, std::string path)
{
        auto* texture = IMG_LoadTexture(&renderer, path.c_str());

        if (!texture)
                throw Error();

        return Unique_texture(texture);
}

Dimensions texture_dimensions(Texture& texture)
{
        return {
                texture_width(texture),
                texture_height(texture)
        };
}

int texture_width(Texture& texture)
{
        int width;
        if (SDL_QueryTexture(&texture, nullptr, nullptr, &width, nullptr) < 0)
                throw Error();
        return width;
}

int texture_height(Texture& texture)
{
        int height;
        if (SDL_QueryTexture(&texture, nullptr, nullptr, nullptr, &height) < 0)
                throw Error();
        return height;
}

Duration::Milliseconds get_ticks() noexcept
{
        return Duration::Milliseconds {SDL_GetTicks()};
}

Optional_event poll_event()
{
        Event event;
        if (SDL_PollEvent(&event))
                return event;
        return std::nullopt;
}

}

