#pragma once

#include <string>
#include "sdl++.h"
#include "base.h"

namespace Engine::Graphics {

struct Sprite {
public:
        Sprite(Sdl::Texture& texture, int frame_count);

        void render_frame(Sdl::Renderer& renderer,
                          Complex_number position,
                          int frame_num,
                          double angle=0,
                          Sdl::Flip flip=Sdl::Flip::none);

        int frame_count() const noexcept; 

private:
        Sdl::Rect source_rect(int frame_num) const noexcept;
        Sdl::Rect destination_rect(Complex_number position) const noexcept;

        Sdl::Texture* texture_;
        int frame_count_;
        int frame_width_ = Sdl::texture_width(*texture_) / frame_count_;
        int frame_height_ = Sdl::texture_height(*texture_);
};

class Animation {
public:
        Animation(Sprite& sprite, int frame_delay) noexcept;

        void advance() noexcept;
        void render(Sdl::Renderer& renderer,
                    Complex_number position,
                    double angle=0,
                    Sdl::Flip flip=Sdl::Flip::none);

private:
        void advance_frame_num() noexcept;

        Sprite* sprite_;
        int remaining_frame_delay_;
        int max_frame_delay_;
        int frame_num_ = 0;
};

struct Animated_sprite {
        static Animated_sprite load(Sdl::Renderer& renderer,
                                    std::string const& base_path,
                                    std::string const& image_extension="png",
                                    std::string const& config_extension="sheet.config");

        Sprite sprite;
        Animation animation;
};

}

