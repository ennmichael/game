#pragma once

#include "sdl++.h"
#include "base.h"
#include "gameplay.h"
#include <unordered_map>
#include <string>

/**
 * TODO Consider if the `Timer` should be extracted and shared between the gameplay and 
 * graphics modules. Currently, it's in the gameplay module, but that makes graphics
 * dependent on gameplay.
 */

namespace Engine::Graphics {

class Sprite_sheet {
public:
        class Sprite {
        public:
                Sprite(Sprite_sheet& sheet,
                       Sdl::Rect source) noexcept;

                void render(Sdl::Renderer& renderer,
                            Complex_number position,
                            Sdl::Flip flip=Sdl::Flip::none);

        private:
                Sprite_sheet& sprite_sheet_;
                Sdl::Rect source_;
        };

        class Animation {
        public:
                Animation(Sprite_sheet& sheet,
                          Sdl::Rect first_frame_source,
                          Duration::Milliseconds frame_delay,
                          int frame_count) noexcept;

                void render(Sdl::Renderer&,
                            Complex_number position,
                            Sdl::Flip flip=Sdl::Flip::none);
                void update() noexcept;

        private:
                Sprite_sheet& sprite_sheet_;
                Sdl::Rect source_;
                Gameplay::Timer timer_;
                int frame_count_;
                int current_frame_ = 0;
        };

        Sprite_sheet(Sdl::Unique_texture texture);

        Sprite sprite(Sdl::Rect source) noexcept;
        Animation animation(Sdl::Rect first_frame_source,
                            Duration::Milliseconds frame_delay,
                            int frame_count) noexcept;

private:
        void render(Sdl::Renderer& renderer,
                    Sdl::Rect source,
                    Sdl::Rect destination,
                    Sdl::Flip flip=Sdl::Flip::none);

        Sdl::Unique_texture texture_;
};

using Sprite = Sprite_sheet::Sprite;
using Animation = Sprite_sheet::Animation;

}

