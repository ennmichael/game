#include "graphics.h"
#include "utils.h"
#include <utility>
#include <type_traits>

using namespace std::string_literals;

namespace Engine::Graphics {

namespace Detail {

Sprite_sheet::Sprite::Sprite(Raw_sprite_sheet& sprite_sheet,
                             Sdl::Rect source) noexcept
        : sprite_sheet_(&sprite_sheet)
        , source_(source)
{}

void Sprite_sheet::Sprite::render(Sdl::Renderer& renderer,
                                  Complex_number position,
                                  Sdl::Flip flip)
{
        auto const dst = Sdl::make_rect(Sdl::Dimensions(source_), position);
        sprite_sheet_->render(renderer, source_, dst, flip);
}

Raw_sprite_sheet::Animation::Animation(Raw_sprite_sheet& sprite_sheet,
                                       Sdl::Rect first_frame_source,
                                       Duration::Milliseconds frame_delay,
                                       int frame_count) noexcept
        : sprite_sheet_(&sprite_sheet)
        , source_(first_frame_source)
        , timer_(frame_delay)
        , frame_count_(frame_count)
{}

void Sprite_sheet::Animation::update() noexcept
{
        if (timer_.ready())
                ++current_frame_;

        if (current_frame_ == frame_count_)
                current_frame_ = 0;
}

void Sprite_sheet::Animation::render(Sdl::Renderer& renderer,
                                     Complex_number position,
                                     Sdl::Flip flip)
{
        auto const dst = make_rect(Sdl::Dimensions(source_), position);
        sprite_sheet_->render(renderer, source_, dst, flip);
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

}

Sprite_sheet::Sprite_sheet(Sdl::Renderer& renderer,
                           std::string const& image_path,
                           std::string const& sprite_sheet_json_path,
                           std::string const& animations_json_path)
        : Sprite_sheet(Sdl::load_texture(renderer, image_path),
                       sprite_sheet_json_path,
                       animations_json_path)
{}

Sprite_sheet::Sprite_sheet(Sdl::Unique_texture texture,
                           std::string const& sprite_sheet_json_path,
                           std::string const& animations_json_path)
        : Sprite_sheet(std::move(texture),
                       read_json_tree(sprite_sheet_json_path),
                       read_json_tree(animations_json_path))
{}

Sprite_sheet::Sprite_sheet(Sdl::Unique_texture texture,
                           boost::property_tree::ptree sprite_sheet_tree,
                           boost::property_tree::ptree animations_tree)
        : sprite_sheet_(std::move(texture))
        , sprite_sheet_tree_(std::move(sprite_sheet_tree))
        , animations_tree_(std::move(animations_tree))
{}

auto Sprite_sheet::sprite(std::string const& name) noexcept -> Sprite
{
        auto const src = sprite_source_rect(name);
        return sprite_sheet_.sprite(src);
}

auto Sprite_sheet::animation(std::string const& name) noexcept -> Animation
{
        auto const [frame_count, frame_delay] = animation_frame_properties(name);
        return sprite_sheet_.animation(animation_first_rect(name, frame_count),
                                       frame_delay,
                                       frame_count);
}

Sdl::Rect Sprite_sheet::sprite_source_rect(std::string const& name) const
{
        auto const sprite_props = sprite_sheet_tree_.get_child(name);
        return {
                sprite_props.get<int>("x"s),
                sprite_props.get<int>("y"s),
                sprite_props.get<int>("width"s),
                sprite_props.get<int>("height"s)
        };
}

Sdl::Rect Sprite_sheet::animation_first_rect(std::string const& name, int frame_count) const
{
        auto sprite_rect = sprite_source_rect(name);
        sprite_rect.w /= frame_count;
        return sprite_rect;
}

auto Sprite_sheet::animation_frame_properties(std::string const& name) const -> Animation_frame_properties
{
        auto const animation_props = animations_tree_.get_child(name);

        return {
                animation_props.get<int>("frame_count"s),
                Duration::Milliseconds {animation_props.get<unsigned>("frame_delay"s)}
        };
}

}

