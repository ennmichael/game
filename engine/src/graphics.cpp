#include "graphics.h"
#include "utils.h"
#include <utility>
#include <type_traits>

using namespace std::string_literals;

namespace Engine::Graphics {

Sprite_sheet::Sprite::Sprite(Sprite_sheet& sprite_sheet,
                             Sdl::Rect source) noexcept
        : sprite_sheet_(&sprite_sheet)
        , source_(source)
{}

void Sprite_sheet::Sprite::render(Sdl::Renderer& renderer,
                                  Complex_number position,
                                  Sdl::Flip flip)
{
        auto const dst = Sdl::make_rect(position, Sdl::rect_dimensions(source_));
        sprite_sheet_->render(renderer, source_, dst, flip);
}

Sprite_sheet::Animation::Animation(Sprite_sheet& sprite_sheet,
                                   Sdl::Rect whole_source,
                                   int frame_count,
                                   Duration::Frames frame_delay) noexcept
        : sprite_sheet_(&sprite_sheet)
        , source_ {whole_source.x, whole_source.y, whole_source.w/frame_count, whole_source.h}
        , frame_delay_(frame_delay)
        , frame_count_(frame_count)
{}

void Sprite_sheet::Animation::render_frame(Sdl::Renderer& renderer,
                                           int frame,
                                           Complex_number position,
                                           Sdl::Flip flip)
{
        auto const dst = Sdl::make_rect(position, Sdl::rect_dimensions(source_));
        auto const source_x = source_.x + source_.w*frame;
        sprite_sheet_->render(renderer,
                              Sdl::Rect {source_x, source_.y, source_.w, source_.h},
                              dst,
                              flip);
}

Duration::Frames Sprite_sheet::Animation::duration() const noexcept
{
        return frame_delay_ * Duration::Frames {frame_count_};
}

Duration::Frames Sprite_sheet::Animation::frame_delay() const noexcept
{
        return frame_delay_;
}

int Sprite_sheet::Animation::frame_count() const noexcept
{
        return frame_count_;
}

Sprite_sheet::Sprite_sheet(Sdl::Unique_texture texture) noexcept
        : texture_(std::move(texture))
{}

Sprite_sheet::Sprite_sheet(Sdl::Renderer& renderer,
                           std::string const& path)
        : Sprite_sheet(Sdl::load_texture(renderer, path))
{}

Sprite Sprite_sheet::sprite(Sdl::Rect source) noexcept
{
        return Sprite(*this, source);
}

Animation Sprite_sheet::animation(Sdl::Rect first_frame_source,
                                  int frame_count,
                                  Duration::Frames frame_delay) noexcept
{
        return Animation(*this, first_frame_source, frame_count, frame_delay);
}

void Sprite_sheet::render(Sdl::Renderer& renderer,
                          Sdl::Rect source,
                          Sdl::Rect destination,
                          Sdl::Flip flip)
{
        Sdl::render_copy(renderer,
                         *texture_,
                         source,
                         destination,
                         0,
                         flip);
}

Sprite_sheet_config load_sprite_sheet_config(std::string const& json_path)
{
        Sprite_sheet_config config;

        for (auto const& [sprite_name, props] : Engine::Utils::read_json_tree(json_path))
                config[sprite_name] = {
                        props.get<int>("x"s),
                        props.get<int>("y"s),
                        props.get<int>("width"s),
                        props.get<int>("height"s)
                };

        return config;
}

Animations_config load_animations_config(std::string const& json_path)
{
        Animations_config config;

        for (auto const& [sprite_name, props] : Engine::Utils::read_json_tree(json_path))
                config[sprite_name] = {
                        props.get<int>("frame_count"s),
                        Duration::Frames {props.get<int>("frame_delay")}
                };

        return config;
}

Animations animations(Sprite_sheet& sprite_sheet,
                      Sprite_sheet_config const& sprite_sheet_config,
                      Animations_config const& animations_config)
{
        Animations animations;

        for (auto const& [name, props] : animations_config) {
                auto const [frame_count, frame_delay] = props;
                auto const source = sprite_sheet_config.at(name);
                auto const animation = sprite_sheet.animation(source, frame_count, frame_delay);
                animations.try_emplace(name, animation);
        }

        return animations;
}

Current_animation::Current_animation(Animation& animation) noexcept
        : animation_(&animation)
        , frame_timer_(animation.frame_delay())
{}

void Current_animation::update() noexcept
{
        frame_timer_.update();

        if (frame_timer_.ready()) {
                frame_timer_.restart();
                ++current_frame_;
        }

        if (current_frame_ == animation_->frame_count())
                current_frame_ = 0;
}

void Current_animation::hard_switch(Animation& new_animation) noexcept
{
        *this = Current_animation(new_animation);
}

void Current_animation::soft_switch(Animation& new_animation) noexcept
{
        animation_ = &new_animation;
}

void Current_animation::render(Sdl::Renderer& renderer,
                        Complex_number position,
                        Sdl::Flip flip)
{
        animation_->render_frame(renderer, current_frame_, position, flip);
}

void apply_filter(Sdl::Renderer& renderer, Color color)
{
        Sdl::Renderer_color_lock color_lock(renderer, color);
        (void)color_lock;

        auto const dimensions = Sdl::renderer_dimensions(renderer);
        auto const rect = Sdl::make_rect(Complex_number(), dimensions);
        return Sdl::render_rect(renderer, rect);
}

}

