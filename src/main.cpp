#include "engine/graphics.h"
#include <string>



#include <iostream>






using namespace std::string_literals;
using namespace std::complex_literals;

int main()
{
        auto const [window, renderer] = Sdl::create_window_and_renderer("Title"s, 500, 500);

        auto spr = Engine::Graphics::Sprite::load_from_file(renderer, "../res/sprites/run"s);
        auto animation = spr.animation();

        auto position = 0.0 + 150.0i;

        auto quit = false;
        while (!quit) {
                position += 0.04;
                animation.frame_advance();

                Sdl::render_clear(renderer);
                spr.render(renderer, position);
                Sdl::render_present(renderer);

                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT)
                                quit = true;
                }
        }

        (void)window; // Perhaps this should be burried somewhere inside Engine?
}

