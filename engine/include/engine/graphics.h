#pragma once

#include "sdl++.h"
#include "base.h"
#include "gameplay.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include <unordered_map>
#include <string>

/**
 * TODO Consider if the `Timer` should be extracted and shared between the gameplay and 
 * graphics modules. Currently, it's in the gameplay module, but that makes graphics
 * dependent on gameplay.
 */

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
                          Duration::Milliseconds frame_delay,
                          int frame_count) noexcept;

                void render(Sdl::Renderer&,
                            Complex_number position,
                            Sdl::Flip flip=Sdl::Flip::none);
                void update() noexcept;

        private:
                Raw_sprite_sheet* sprite_sheet_;
                Sdl::Rect source_;
                Gameplay::Timer timer_;
                int frame_count_;
                int current_frame_ = 0;
        };

        explicit Raw_sprite_sheet(Sdl::Unique_texture texture) noexcept;

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

} // Close namespace Detail

class Sprite_sheet {
public:
        using Sprite = Detail::Raw_sprite_sheet::Sprite;
        using Animation = Detail::Raw_sprite_sheet::Animation;

        Sprite_sheet(Sdl::Renderer& renderer,
                     std::string const& image_path,
                     std::string const& sprite_sheet_json_path,
                     std::string const& animations_json_path);
        Sprite_sheet(Sdl::Unique_texture texture,
                     std::string const& sprite_sheet_json_path,
                     std::string const& animations_json_path);
        Sprite_sheet(Sdl::Unique_texture texture,
                     boost::property_tree::ptree sprie_sheet_tree,
                     boost::property_tree::ptree animations_tree);

        Sprite sprite(std::string const& name) noexcept;
        Animation animation(std::string const& name) noexcept;

private:
        struct Animation_frame_properties {
                int frame_count;
                Duration::Milliseconds frame_delay;
        };

        Sdl::Rect sprite_source_rect(std::string const& name) const;
        Sdl::Rect animation_first_rect(std::string const& name, int frame_count) const;
        Animation_frame_properties animation_frame_properties(std::string const& name) const;

        Detail::Raw_sprite_sheet sprite_sheet_;
        boost::property_tree::ptree sprite_sheet_tree_;
        boost::property_tree::ptree animations_tree_;
};

using Sprite = Sprite_sheet::Sprite;
using Animation = Sprite_sheet::Animation;

}

