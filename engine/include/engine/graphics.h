#pragma once

#include "sdl++.h"
#include "base.h"
#include "gameplay.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include <unordered_map>
#include <string>

namespace Engine::Graphics {

namespace Detail {

class Raw_sprite_sheet {
public:
        class Sprite {
        public:
                Sprite(Raw_sprite_sheet& sheet,
                       Sdl::Rect source) noexcept;

                void render(Sdl::Renderer& renderer,
                            Complex_number position,
                            Sdl::Flip flip=Sdl::Flip::none);

        private:
                Raw_sprite_sheet* sprite_sheet_;
                Sdl::Rect source_;
        };

        class Animation {
        public:
                Animation(Raw_sprite_sheet& sheet,
                          Sdl::Rect first_frame_source,
                          Duration::Frames frame_delay,
                          int frame_count) noexcept;

                void render(Sdl::Renderer&,
                            Complex_number position,
                            Sdl::Flip flip=Sdl::Flip::none);
                void update() noexcept;

                Sdl::Dimensions frame_dimensions() const noexcept;

        private:
                Raw_sprite_sheet* sprite_sheet_;
                Sdl::Rect source_;
                Gameplay::Frame_timer timer_;
                int frame_count_;
                int current_frame_ = 0;
        };

        explicit Raw_sprite_sheet(Sdl::Unique_texture texture) noexcept;

        Sprite sprite(Sdl::Rect source) noexcept;
        Animation animation(Sdl::Rect first_frame_source,
                            Duration::Frames frame_delay,
                            int frame_count) noexcept;

private:
        void render(Sdl::Renderer& renderer,
                    Sdl::Rect source,
                    Sdl::Rect destination,
                    Sdl::Flip flip=Sdl::Flip::none);

        Sdl::Unique_texture texture_;
};

} // Close namespace Detail

struct Animation_properties {
        Duration::Frames animation_duration() const noexcept;

        int frame_count;
        Duration::Frames frame_delay;
};

using Sprite_sheet_config = std::unordered_map<std::string, Sdl::Rect>;

Sprite_sheet_config load_sprite_sheet_config(std::string const& json_path);

using Animations_config = std::unordered_map<std::string, Animation_properties>;
using Animations_durations = std::unordered_map<std::string, Duration::Frames>;

Animations_config load_animations_config(std::string const& json_path);
Animations_durations animations_durations(Animations_config const& animations_config);

class Sprite_sheet {
public:
        using Sprite = Detail::Raw_sprite_sheet::Sprite;
        using Animation = Detail::Raw_sprite_sheet::Animation;

        Sprite_sheet(Sdl::Unique_texture texture,
                     Sprite_sheet_config sprite_sheet_config,
                     Animations_config animations_config);
        Sprite_sheet(Sdl::Renderer& renderer,
                     std::string const& image_path,
                     Sprite_sheet_config sprite_sheet_config,
                     Animations_config animations_config);

        Sprite sprite(std::string const& name);
        Animation animation(std::string const& name);

private:
        Sdl::Rect animation_first_rect(std::string const& name, int frame_count) const;

        Detail::Raw_sprite_sheet sprite_sheet_;
        Sprite_sheet_config sprite_sheet_config_;
        Animations_config animations_config_;
};

using Sprite = Sprite_sheet::Sprite;
using Animation = Sprite_sheet::Animation;

}

