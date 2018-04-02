#pragma once

#include "sdl++.h"
#include "base.h"
#include "gameplay.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include <unordered_map>
#include <string>

/*
 * TODO Instead of `Sprite`, introduce the concept of a `Static_sprite`
 */

namespace Engine::Graphics {

class Animations {
public:
        Animations()

private:
        
};

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
                Sprite_sheet* sprite_sheet_;
                Sdl::Rect source_;
        };

        class Animation {
        public:
                Animation(Sprite_sheet& sheet,
                          Sdl::Rect first_frame_source,
                          Duration::Frames frame_delay,
                          int frame_count) noexcept;

                void render(Sdl::Renderer&,
                            Complex_number position,
                            Sdl::Flip flip=Sdl::Flip::none);
                void update() noexcept;

                Dimensions frame_dimensions() const noexcept;

        private:
                Sprite_sheet* sprite_sheet_;
                Sdl::Rect source_;
                Gameplay::Frame_timer timer_;
                int frame_count_;
                int current_frame_ = 0;
        };

        explicit Sprite_sheet(Sdl::Unique_texture texture) noexcept;
        Sprite_sheet(Sdl::Renderer& renderer,
                     std::string const& path);

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

using Sprite = Sprite_sheet::Sprite;
using Animation = Sprite_sheet::Animation;

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

using Animations = std::unordered_map<std::string, Animation>;
using Sprites = std::unordered_map<std::string, Sprite>;

struct Resources {
        Animations animations;
        Sprites sprites;
};

Resources load_resources(Sprite_sheet& sprite_sheet,
                         Sprite_sheet_config const& sprite_sheet_config,
                         Animations_config const& animations_config);

void apply_filter(Sdl::Renderer& renderer, Color color);

}

