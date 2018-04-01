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

Unique_window create_window(std::string const& title, int width, int height)
{
        auto* window = SDL_CreateWindow(title.c_str(),
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        width,
                                        height,
                                        0);

        if (!window)
                throw Error();

        return Unique_window(window);
}

Unique_renderer create_renderer(Window& window)
{
        auto* renderer = SDL_CreateRenderer(&window, -1, 0);

        if (!renderer)
                throw Error();

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        return Unique_renderer(renderer);
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
                 Rect source,
                 Rect destination)
{
        render_copy(renderer, texture, source, destination, 0, Flip::none);
}

void render_copy(Renderer& renderer,
                 Texture& texture,
                 Complex_number position,
                 double angle,
                 Flip flip)
{
        auto const dimensions = texture_dimensions(texture);
        auto const src = make_rect(Complex_number(0, 0), dimensions);
        auto const dst = make_rect(position, dimensions);

        render_copy(renderer, texture, src, dst, angle, flip);
}

Unique_texture load_texture(Renderer& renderer, std::string path)
{
        auto* texture = IMG_LoadTexture(&renderer, path.c_str());

        if (!texture)
                throw Error();

        return Unique_texture(texture);
}

bool has_intersection(Rect r1, Rect r2) noexcept
{
        return SDL_HasIntersection(&r1, &r2);
}

Rect make_rect(Complex_number position, Dimensions dimensions) noexcept
{
        auto const [w, h] = dimensions;

        return {
                static_cast<int>(position.real()),
                static_cast<int>(position.imag()),
                w, h
        };
}

Dimensions rect_dimensions(Rect rect) noexcept
{
        return {rect.w, rect.h};
}

Dimensions texture_dimensions(Texture& texture)
{
        return Dimensions {
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

