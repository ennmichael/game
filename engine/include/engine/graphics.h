#pragma once

#include <string>
#include "sdl++.h"
#include "base.h"

namespace Engine::Graphics {

struct Sprite {
public:
        Sprite(Sdl::Texture texture, int frame_count);

        void render_frame(Sdl::Renderer& renderer,
                          Complex_number position,
                          int frame_num,
                          double angle=0,
                          Sdl::Flip flip=Sdl::Flip::none);

private:
        Sdl::Rect source_rect(int frame_num) const noexcept;
        Sdl::Rect destination_rect(Complex_number position) const noexcept;

        Sdl::Texture texture_;
        int frame_width_;
        int frame_height_;
};

class Animation {
public:
        Animation(int frame_count, int frame_delay) noexcept;

        void update() noexcept;
        int current_frame() const noexcept;

private:
        void update_current_frame() noexcept;

        int remaining_frame_delay_;
        int max_frame_delay_;
        int frame_count_;
        int current_frame_ = 0;
};

struct Animated_sprite {
public:
        static Animated_sprite load(Sdl::Renderer& renderer,
                                    std::string const& base_path,
                                    std::string const& image_extension="png",
                                    std::string const& config_extension="sheet.config");

        Animated_sprite(Sprite sprite, Animation const& animation) noexcept;

        void update();
        void render(Sdl::Renderer& renderer,
                    Complex_number position,
                    double angle=0,
                    Sdl::Flip flip=Sdl::Flip::none);

private:
        Sprite sprite_;
        Animation animation_;
};

}

