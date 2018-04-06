#pragma once

#include "sdl++.h"
#include "base.h"
#include "gameplay.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include <unordered_map>
#include <string>

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
                Sprite_sheet* sprite_sheet_;
                Sdl::Rect source_;
        };

        class Animation {
        public:
                Animation(Sprite_sheet& sheet,
                          Sdl::Rect first_frame_source,
                          int frame_count,
                          Duration::Frames frame_delay) noexcept;

                void render_frame(Sdl::Renderer&,
                                  int frame,
                                  Complex_number position,
                                  Sdl::Flip flip=Sdl::Flip::none);
                
                Duration::Frames duration() const noexcept;
                Duration::Frames frame_delay() const noexcept;
                int frame_count() const noexcept;

        private:
                Sprite_sheet* sprite_sheet_;
                Sdl::Rect source_;
                Duration::Frames frame_delay_;
                int frame_count_;
        };

        explicit Sprite_sheet(Sdl::Unique_texture texture) noexcept;
        Sprite_sheet(Sdl::Renderer& renderer,
                     std::string const& path);

        Sprite sprite(Sdl::Rect source) noexcept;
        Animation animation(Sdl::Rect whole_source,
                            int frame_count,
                            Duration::Frames frame_delay) noexcept;

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
        int frame_count;
        Duration::Frames frame_delay;
};

using Sprite_sheet_config = std::unordered_map<std::string, Sdl::Rect>;
using Animations_config = std::unordered_map<std::string, Animation_properties>;

Sprite_sheet_config load_sprite_sheet_config(std::string const& json_path);
Animations_config load_animations_config(std::string const& json_path);

using Animations = std::unordered_map<std::string, Animation>;
Animations animations(Sprite_sheet& sprite_sheet,
                      Sprite_sheet_config const& sprite_sheet_config,
                      Animations_config const& animations_config);

class Current_animation {
public:
        explicit Current_animation(Animation& animation) noexcept;

        void hard_switch(Animation& new_animation) noexcept; /* Resets the current frame */
        void soft_switch(Animation& new_animation) noexcept; /* Doesn't reset the current frame */
        void render(Sdl::Renderer& renderer,
                    Complex_number position,
                    Sdl::Flip flip=Sdl::Flip::none);


        void update() noexcept;
        bool is_finished() const noexcept;
        void loop() noexcept;

private:
        using Animations = std::unordered_map<std::string, Animation>;

        Animation* animation_;
        Gameplay::Frame_timer frame_timer_;
        int current_frame_=0;
};

void apply_filter(Sdl::Renderer& renderer, Color color);

}

