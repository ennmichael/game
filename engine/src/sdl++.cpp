#include "sdl++.h"
#include "SDL_image.h"
#include <type_traits>

namespace Engine::Sdl {

void Window_deleter::operator()(SDL_Window* window) const noexcept
{
        SDL_DestroyWindow(window);
}

void Renderer_deleter::operator()(SDL_Renderer* renderer) const noexcept
{
        SDL_DestroyRenderer(renderer);
}

void Texture_deleter::operator()(SDL_Texture* texture) const noexcept
{
        SDL_DestroyTexture(texture);
}

char const* Error::what() const noexcept
{
        return SDL_GetError();
}

Manager::Manager()
{
        SDL_Init(SDL_INIT_EVERYTHING);
}

Manager::~Manager()
{
        SDL_Quit();
}

WindowAndRenderer create_window_and_renderer(
        std::string const& title, 
        int width, 
        int height,
        Color background_color)
{ // TODO Proper error checking
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;

        SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);

        SDL_SetWindowTitle(window, title.c_str());
        SDL_SetRenderDrawColor(renderer, 
                               background_color.r,
                               background_color.g,
                               background_color.b,
                               background_color.a);

        return WindowAndRenderer(Window(window), Renderer(renderer));
}

void render_clear(Renderer const& renderer)
{ // TODO Error handling
        SDL_RenderClear(renderer.get());
}

void render_present(Renderer const& renderer)
{
        SDL_RenderPresent(renderer.get());
}

void render_copy(Renderer const& renderer, 
                 Texture const& texture, 
                 Rect source, 
                 Rect destination,
                 double angle,
                 Flip flip)
{ // TODO Error handling
        SDL_RenderCopyEx(renderer.get(),
                        texture.get(),
                        &source,
                        &destination,
                        angle,
                        nullptr, // Center point, fine this way
                        static_cast<SDL_RendererFlip>(flip));
}

Texture load_texture(Renderer const& renderer, std::string const& path)
{
        return Texture(IMG_LoadTexture(renderer.get(), path.c_str()));
}

Dimensions texture_dimensions(Texture const& texture)
{
        return {
                texture_width(texture),
                texture_height(texture)
        };
}

int texture_width(Texture const& texture)
{
        int width;
        SDL_QueryTexture(texture.get(), nullptr, nullptr, &width, nullptr);
        return width;
}

int texture_height(Texture const& texture)
{
        int height;
        SDL_QueryTexture(texture.get(), nullptr, nullptr, nullptr, &height);
        return height;
}

int get_ticks() noexcept
{
        return SDL_GetTicks();
}

Optional_event poll_event()
{
        Event event;
        if (SDL_PollEvent(&event))
                return event;
        return std::nullopt;
}

}

