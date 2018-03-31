#include "graphics.h"
#include "utils.h"
#include "boost/range/adaptors.hpp"
#include <utility>
#include <type_traits>

using namespace std::string_literals;

namespace Engine::Graphics {

namespace Detail {

Raw_sprite_sheet::Sprite::Sprite(Raw_sprite_sheet& sprite_sheet,
                             Sdl::Rect source) noexcept
        : sprite_sheet_(&sprite_sheet)
        , source_(source)
{}

void Raw_sprite_sheet::Sprite::render(Sdl::Renderer& renderer,
                                  Complex_number position,
                                  Sdl::Flip flip)
{
        auto const dst = Sdl::make_rect(position, Sdl::Dimensions(source_));
        sprite_sheet_->render(renderer, source_, dst, flip);
}

Raw_sprite_sheet::Animation::Animation(Raw_sprite_sheet& sprite_sheet,
                                       Sdl::Rect first_frame_source,
                                       Duration::Frames frame_delay,
                                       int frame_count) noexcept
        : sprite_sheet_(&sprite_sheet)
        , source_(first_frame_source)
        , timer_(frame_delay)
        , frame_count_(frame_count)
{}

void Raw_sprite_sheet::Animation::update() noexcept
{
        timer_.update();

        if (timer_.ready()) {
                timer_.restart();
                ++current_frame_;
        }

        if (current_frame_ == frame_count_)
                current_frame_ = 0;
}

void Raw_sprite_sheet::Animation::render(Sdl::Renderer& renderer,
                                     Complex_number position,
                                     Sdl::Flip flip)
{
        auto const dst = make_rect(position, Sdl::Dimensions(source_));
        sprite_sheet_->render(renderer, source_, dst, flip);
}

Sdl::Dimensions Raw_sprite_sheet::Animation::frame_dimensions() const noexcept
{
        return Sdl::Dimensions(source_);
}

Raw_sprite_sheet::Raw_sprite_sheet(Sdl::Unique_texture texture) noexcept
        : texture_(std::move(texture))
{}

Sprite Raw_sprite_sheet::sprite(Sdl::Rect source) noexcept
{
        return Sprite(*this, source);
}

void Raw_sprite_sheet::render(Sdl::Renderer& renderer,
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

} // Close namespace Detail

namespace {

boost::property_tree::ptree read_json_tree(const std::string& path)
{
        boost::property_tree::ptree result;
        boost::property_tree::read_json(path, result);
        return result;
}

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

}

Duration::Frames Animation_properties::animation_duration() const noexcept
{
        return Duration::Frames {frame_count} * frame_delay;
}

Sprite_sheet_config load_sprite_sheet_config(std::string const& json_path)
{
        return parse_tree(read_json_tree(json_path),
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
        return parse_tree(read_json_tree(json_path),
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

Sprite_sheet::Sprite_sheet(Sdl::Unique_texture texture,
                           Sprite_sheet_config sprite_sheet_config,
                           Animations_config animations_config)
        : sprite_sheet_(std::move(texture))
        , sprite_sheet_config_(std::move(sprite_sheet_config))
        , animations_config_(std::move(animations_config))
{}

Sprite_sheet::Sprite_sheet(Sdl::Renderer& renderer,
                           std::string const& image_path,
                           Sprite_sheet_config sprite_sheet_config,
                           Animations_config animations_config)
        : Sprite_sheet(Sdl::load_texture(renderer, image_path),
                       std::move(sprite_sheet_config),
                       std::move(animations_config))
{}

auto Sprite_sheet::sprite(std::string const& name) -> Sprite
{
        auto const src = sprite_sheet_config_.at(name);
        return sprite_sheet_.sprite(src);
}

auto Sprite_sheet::animation(std::string const& name) -> Animation
{
        auto const [frame_count, frame_delay] = animations_config_.at(name);
        return sprite_sheet_.animation(animation_first_rect(name, frame_count),
                                       frame_delay,
                                       frame_count);
}

Sdl::Rect Sprite_sheet::animation_first_rect(std::string const& name, int frame_count) const
{
        auto sprite_rect = sprite_sheet_config_.at(name);
        sprite_rect.w /= frame_count;
        return sprite_rect;
}

}

