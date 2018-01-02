#pragma once

#include <string>
#include "sdl++.h"
#include "base.h"

namespace Engine::Graphics {

using Frame = Sdl::Rect;

class Frame {
public:
        Frame(Sdl::Rect source_rect, Sdl::Texture& texture) noexcept;

        void render(Sdl::Renderer& renderer,
                    Complex_number position,
                    double angle=0,
                    Sdl::Flip flip=Sdl::Flip::none);

private:
        Sdl::Texture* texture_;
        Sdl::Rect source_rect_;
};

using Sprite = std::vector<Frame>;

Sprite create_sprite(Sdl::Texture& texture, int frame_count);

class Animation {
public:
        Animation(Sprite const& sprite, int frame_delay) noexcept;

        void advance() noexcept;
        void render(Sdl::Renderer& renderer,
                    Complex_number position,
                    double angle=0,
                    Sdl::Flip flip=Sdl::Flip::none);

private:
        void advance_frame() noexcept;

        Sprite const* sprite_;
        Sprite::const_iterator current_frame_ = sprite_.cbegin();
        int remaining_frame_delay_;
        int max_frame_delay_;
};

std::pair<Sprite, Animation> load_graphics();

struct AnimatedSprite {
        static AnimatedSprite load(std::string const& base_path,
                                   std::string const& image_extension="png",
                                   std::string const& config_extension="sheet.config");

        Sprite sprite;
        Animation animation;
};

}

