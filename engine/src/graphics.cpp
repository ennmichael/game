#include "graphics.h"
#include "utils.h"
#include "boost/range/adaptors.hpp"
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
                                   Sdl::Rect first_frame_source,
                                   Duration::Frames frame_delay,
                                   int frame_count) noexcept
        : sprite_sheet_(&sprite_sheet)
        , source_(first_frame_source)
        , timer_(frame_delay)
        , frame_count_(frame_count)
{}

void Sprite_sheet::Animation::update() noexcept
{
        timer_.update();

        if (timer_.ready()) {
                timer_.restart();
                ++current_frame_;
        }

        if (current_frame_ == frame_count_)
                current_frame_ = 0;
}

void Sprite_sheet::Animation::render(Sdl::Renderer& renderer,
                                     Complex_number position,
                                     Sdl::Flip flip)
{
        auto const dst = Sdl::make_rect(position, Sdl::rect_dimensions(source_));
        auto const source_x = source_.x + source_.w*current_frame_;
        sprite_sheet_->render(renderer,
                              Sdl::Rect {source_x, source_.y, source_.w, source_.h},
                              dst,
                              flip);
}

Dimensions Sprite_sheet::Animation::frame_dimensions() const noexcept
{
        return Sdl::rect_dimensions(source_);
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
                                  Duration::Frames frame_delay,
                                  int frame_count) noexcept
{
        return Animation(*this, first_frame_source, frame_delay, frame_count);
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

namespace {

template <class TransformationFunction>
auto parse_tree(boost::property_tree::ptree tree, TransformationFunction const& f)
{
        auto const range = tree | boost::adaptors::transformed(
                [&](boost::property_tree::ptree::value_type node)
                {
                        auto const& [key, value] = node;
                        return std::pair(key, f(value));
                });

        using Value = std::invoke_result_t<TransformationFunction,
                                           boost::property_tree::ptree>;

        return std::unordered_map<std::string, Value>(range.begin(), range.end());
}

} // Close unnamed namespace

Duration::Frames Animation_properties::animation_duration() const noexcept
{
        return Duration::Frames {frame_count} * frame_delay;
}

Sprite_sheet_config load_sprite_sheet_config(std::string const& json_path)
{
        return parse_tree(Utils::read_json_tree(json_path),
                          [](boost::property_tree::ptree child)
                          {
                                  return Sdl::Rect {
                                          child.get<int>("x"s),
                                          child.get<int>("y"s),
                                          child.get<int>("width"s),
                                          child.get<int>("height"s)
                                  };
                          });
}

Animations_config load_animations_config(std::string const& json_path)
{
        return parse_tree(Utils::read_json_tree(json_path),
                          [](boost::property_tree::ptree child)
                          {
                                  return Animation_properties {
                                          child.get<int>("frame_count"s),
                                          Duration::Frames {child.get<int>("frame_delay"s)}
                                  };
                          });
}

Animations_durations animations_durations(Animations_config const& animations_config)
{
        auto const range = animations_config | boost::adaptors::transformed(
                [](auto const& node)
                {
                        auto const& [name, properties] = node;
                        return std::pair(name, properties.animation_duration());
                });

        return Animations_durations(range.begin(), range.end());
}

Resources load_resources(Sprite_sheet& sprite_sheet,
                         Sprite_sheet_config const& sprite_sheet_config,
                         Animations_config const& animations_config)
{
        auto const is_animated =
        [&](auto const& sprite_name)
        {
                return animations_config.count(sprite_name) != 0;
        };

        Animations animations;

        auto const add_animation =
        [&](auto const& sprite_name, auto sprite_rect)
        {
                auto const [frame_count, frame_delay] = animations_config.at(sprite_name);
                sprite_rect.w /= frame_count;
                animations.try_emplace(sprite_name,
                                       sprite_sheet.animation(sprite_rect,
                                                              frame_delay,
                                                              frame_count));
        };

        Sprites static_sprites;

        auto const add_static_sprite =
        [&](auto const& sprite_name, auto const& sprite_rect)
        {
                static_sprites.try_emplace(sprite_name, sprite_sheet.sprite(sprite_rect));
        };

        for (auto const& [sprite_name, sprite_rect] : sprite_sheet_config) {
                if (is_animated(sprite_name))
                        add_animation(sprite_name, sprite_rect);
                else
                        add_static_sprite(sprite_name, sprite_rect);
        }

        return {
                animations,
                static_sprites
        };
}

}

