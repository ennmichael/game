#pragma once

#include <string>
#include "sdl++/sdl++.h"
#include "base.h"

namespace Engine::Graphics {

class Animation;

class Sprite {
public:
        static Sprite load_from_file(Sdl::Renderer const& renderer,
                                     std::string const& base_path);

        Sprite(Sdl::Texture texture, int frame_count, int frame_delay) noexcept;
        
        void frame_advance() noexcept;
        int frame_delay() const noexcept;
        Animation animation() noexcept;

        void render(Sdl::Renderer const& renderer,
                    Engine::Complex_number position) noexcept; 

private:
        Sdl::Rect source_rect() const noexcept;
        Sdl::Rect destination_rect(Engine::Complex_number position) const noexcept;

        Sdl::Texture texture_;
        int frame_count_;
        int frame_delay_;
        int current_frame_ = 0;
        int frame_width_;
        int frame_height_;
};

class Animation {
public:
        explicit Animation(Sprite& sprite) noexcept;

        void frame_advance() noexcept;

private:
        Sprite* sprite_;
        int remaining_frame_delay_;
};

}

