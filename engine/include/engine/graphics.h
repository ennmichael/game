#pragma once

#include "sdl++.h"
#include "base.h"
#include <unordered_map>
#include <string>

namespace Engine::Graphics {

struct Sprite {
public:
        Sprite(Sdl::Unique_texture texture, int frame_count);

        void render_frame(Sdl::Renderer& renderer,
                          Complex_number position,
                          int frame_num,
                          double angle=0,
                          Sdl::Flip flip=Sdl::Flip::none);

        int frame_width() const noexcept;
        int frame_height() const noexcept;

private:
        Sdl::Rect source_rect(int frame_num) const noexcept;
        Sdl::Rect destination_rect(Complex_number position) const noexcept;

        Sdl::Unique_texture texture_;
        int frame_width_;
        int frame_height_;
};

class Animation {
public:
        Animation(int frame_count, Duration::Frames frame_delay) noexcept;

        void update() noexcept;
        int current_frame() const noexcept;

        Duration::Frames duration() const noexcept;

private:
        void update_current_frame() noexcept;

        Duration::Frames max_frame_delay_;
        Duration::Frames remaining_frame_delay_ = max_frame_delay_;
        int frame_count_;
        int current_frame_ {0};
};

struct Animated_sprite {
public:
        static Animated_sprite load(Sdl::Renderer& renderer, // TODO Remove this function
                                    std::string const& base_path,
                                    std::string const& image_extension="png",
                                    std::string const& config_extension="sheet.config");

        Animated_sprite(Sprite sprite, Animation const& animation) noexcept;

        Duration::Frames animation_duration() const noexcept;

        void update();
        void render(Sdl::Renderer& renderer,
                    Complex_number position,
                    double angle=0,
                    Sdl::Flip flip=Sdl::Flip::none);

        int frame_width() const noexcept;
        int frame_height() const noexcept;

private:
        Sprite sprite_;
        Animation animation_;
};

using Animated_sprites = std::unordered_map<std::string, Animated_sprite>;
using Animations_durations = std::unordered_map<std::string, Duration::Frames>;

void load_animated_sprite(Animated_sprites& sprites, Sdl::Renderer& renderer, std::string const& path);

Animations_durations animations_durations(Animated_sprites const& sprites);

}

