#pragma once

#include "sdl++.h"
#include "base.h"
#include "gameplay.h"
#include <unordered_map>
#include <string>

// TODO This file is unfinished

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
                       Sdl::Point source_position,
                       Sdl::Size frame_size) noexcept;

                void render(Sdl::Renderer& renderer, Sdl::Point position);

        private:
                Sprite_sheet& sheet_;
                Sdl::Point source_position_;
        };

        class Animation {
        public:
                Animation(Sprite_sheet& sheet,
                          Sdl::Size frame_size,
                          Duration::Milliseconds frame_delay,
                          int frame_count) noexcept;

                void render(Sdl::Renderer&, Sdl::Point position);
                void update() noexcept;

        private:
                Sprite_sheet& sheet_;
                Sdl::Size frame_size_;
                Gameplay::Timer timer_;
                int frame_count_;
                int current_frame_ = 0;
        };

        Sprite_sheet(Sdl::Renderer& renderer);

        Sprite sprite(Sdl::Point position) const noexcept;
        Animation animation(Duration::Milliseconds frame_delay,
                            int frame_count);

private:
        void render(Sdl::Renderer& renderer,
                    Sdl::Rect source,
                    Sdl::Rect destination);

        Sdl::Unique_texture texture_;
        Sdl::Size frame_size_;
};

class Sprite_sheet {
public:
        static Sprite_sheet load(Sdl::Renderer& renderer,
                                 std::string const& image_path,
                                 std::string const& config_path);

        Sprite_sheet(Sdl::Unique_texture texture,
                     Duration::Milliseconds frame_delay,
                     int frame_count);

        void update() noexcept;
        void render_current_frame(Sdl::Renderer& renderer,
                                  Complex_number position,
                                  Sdl::Flip flip);

private:
        Sdl::Rect source_rect() const noexcept;
        Sdl::Rect destination_rect(Complex_number position) const noexcept;
 
        
        int frame_width_ = Sdl::texture_width(*texture_) / frame_count_;
        int frame_height_ = Sdl::texture_height(*texture_);
};

}

