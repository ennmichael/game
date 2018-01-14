#include "mike.h"
#include "utils.h"
#include <unordered_map>

using namespace std::string_literals;

namespace Game::Logic {

Mike::Mike(Engine::Complex_number position,
           Actions_durations const& durations,
           int width,
           int height) noexcept
        : position_(position)
        , durations_(durations)
        , width_(width)
        , height_(height)
{
        Engine::Config const config("../res/mike.config"s);
        config.unpack_value("speed"s, speed_);
}

bool Mike::is_standing_still() const noexcept
{
        return state_is<Standing_still>();
}

bool Mike::is_running() const noexcept
{
        return state_is<Running>();
}

bool Mike::is_preparing_to_jump_sideways() const noexcept
{
        return state_is<Preparing_to_jump_sideways>();
}

bool Mike::is_jumping_sideways() const noexcept
{
        return state_is<Jumping_sideways>();
}

bool Mike::is_landing_sideways() const noexcept
{
        return state_is<Landing_sideways>();
}

bool Mike::is_jumping_in_place() const noexcept
{
        return state_is<Jumping_in_place>();
}

bool Mike::is_jumping() const noexcept
{
        return is_jumping_sideways() || is_jumping_in_place();
}

bool Mike::is_climbing() const noexcept
{
        return state_is<Climbing>();
}

bool Mike::is_holding_block() const noexcept
{
        return state_is<Holding_block>();
}

bool Mike::is_pushing_block() const noexcept
{
        return state_is<Pushing_block>();
}

bool Mike::is_pulling_block() const noexcept
{
        return state_is<Pulling_block>();
}

bool Mike::is_facing_left() const noexcept
{
        return direction_ == Engine::Gameplay::Direction::left;
}

bool Mike::is_facing_right() const noexcept
{
        return direction_ == Engine::Gameplay::Direction::right;
}

void Mike::snap_to(Engine::Gameplay::Checkbox checkbox) noexcept
{
        auto const new_real =
        [this](Engine::Gameplay::Checkbox checkbox) noexcept
        {
                return (position_.real() > checkbox.position.real()) ?
                        checkbox.position.real() + checkbox.width :
                        checkbox.position.real() - width_;
        };

        position_.real(new_real(checkbox));
}

void Mike::hold_nearest_block(Blocks& blocks) noexcept
{
        Utils::lambda_visit(state_,
                [](Holding_block) noexcept
                {},

                [this](Pushing_block pushing_block) noexcept
                { state_ = Holding_block {pushing_block.block}; },

                [this](Pulling_block pulling_block) noexcept
                { state_ = Holding_block {pulling_block.block}; },

                [this, &blocks](auto const&) noexcept
                {
                        auto const closest_block =
                        [this](Blocks& blocks) noexcept
                        {
                                return std::min_element(blocks.begin(), blocks.end(),
                                        [this](Block b1, Block b2)
                                        {
                                                auto const d1 = Engine::distance(b1, *this);
                                                auto const d2 = Engine::distance(b2, *this);
                                                return d1 < d2;
                                        }
                                );
                        };

                        auto const block_is_close_enough =
                        [this](Block block)
                        {
                                auto const distance = Engine::distance(Engine::Gameplay::center_position(block),
                                                                       Engine::Gameplay::center_position(*this));

                                return distance <=  3*width_ / 5 + block.width/2 + 10;
                        };

                        auto const block = closest_block(blocks);
                        if (block != blocks.cend() && block_is_close_enough(*block)) {
                                turn_to(*block);
                                snap_to(*block);
                                state_ = Holding_block {&(*block)};
                        }
                }
        );
}

void Mike::move_left() noexcept
{
        move_in_direction(Engine::Gameplay::Direction::left);
}

void Mike::move_right() noexcept
{
        move_in_direction(Engine::Gameplay::Direction::right);
}

void Mike::stand_still() noexcept
{
        direction_ = Engine::Gameplay::Direction::none;
        state_ = Standing_still();
}

Engine::Gameplay::Timed_callback Mike::jump() noexcept
{
        auto const jump_in_place =
        [this]
        {
                state_ = Jumping_in_place();
                return stand_still_after(durations_.at("jumping_in_place"s));
        };

        auto const land_sideways =
        [this]
        {
                state_ = Landing_sideways();
                return stand_still_after(durations_.at("landing_sideways"s));
        };

        auto const jump_sideways =
        [this, land_sideways]
        {
                state_ = Preparing_to_jump_sideways();

                return Engine::Gameplay::Timed_callback(
                        [this, land_sideways]
                        {
                                state_ = Jumping_sideways();
                                return Engine::Gameplay::Timed_callback(land_sideways, durations_.at("jumping_sideways"s));
                        },
                        durations_.at("preparing_to_jump_sideways"s)
                );
        };

        if (is_standing_still())
                return jump_in_place();
        else
                return jump_sideways();
}

void Mike::update_position(Engine::Gameplay::Const_checkboxes_pointers const& solid_checkboxes) noexcept
{
        auto const can_be_translated =
        [this](Engine::Gameplay::Direction direction,
               Engine::Gameplay::Const_checkboxes_pointers const& solid_checkboxes) noexcept
        {
                return checkbox().can_be_translated(direction, speed_, solid_checkboxes);
        };

        auto const translate =
        [this](Engine::Gameplay::Direction direction) noexcept
        {
                position_ = Engine::Gameplay::translated_position(position_,
                                                                  direction,
                                                                  speed_);
        };

        auto const pull_block =
        [this, can_be_translated, translate](
                Block& block,
                Engine::Gameplay::Const_checkboxes_pointers const& solid_checkboxes) noexcept
        {
                auto const opposite_direction = Engine::Gameplay::opposite_direction(direction_);
                if (can_be_translated(opposite_direction, solid_checkboxes)) {
                        translate(opposite_direction);
                        block = block.translated(opposite_direction, speed_);
                }
        };

        auto const push_block =
        [this, translate](Block& block,
                          Engine::Gameplay::Const_checkboxes_pointers const& solid_checkboxes) noexcept
        {
                if (block.can_be_translated(direction_, speed_, solid_checkboxes)) {
                        translate(direction_);
                        block = block.translated(direction_, speed_);
                }
        };

        auto translate_if_possible =
        [this, can_be_translated, translate](
                Engine::Gameplay::Const_checkboxes_pointers const& solid_checkboxes) noexcept
        {
                if (can_be_translated(direction_, solid_checkboxes))
                        translate(direction_);
        };

        Utils::lambda_visit(state_,
                [pull_block, &solid_checkboxes](Pulling_block pulling_block) noexcept
                {
                        pull_block(*pulling_block.block, solid_checkboxes);
                },

                [push_block, &solid_checkboxes](Pushing_block pushing_block) noexcept
                {
                        push_block(*pushing_block.block, solid_checkboxes);
                },

                [translate_if_possible, &solid_checkboxes](Running) noexcept
                {
                        translate_if_possible(solid_checkboxes);
                },

                [translate_if_possible, &solid_checkboxes](Jumping_sideways) noexcept
                {
                        translate_if_possible(solid_checkboxes);
                },

                [](auto const&)
                {}
        );
}

Engine::Gameplay::Direction Mike::direction() const noexcept
{
        return direction_;
}

auto Mike::state() const noexcept -> State
{
        return state_;
}

Engine::Complex_number Mike::position() const noexcept
{
        return position_;
}

Engine::Gameplay::Timed_callback Mike::stand_still_after(Engine::Duration::Frames duration)
{
        return Engine::Gameplay::Timed_callback(
                [this]{ stand_still(); },
                duration
        );
}

Engine::Gameplay::Checkbox Mike::checkbox() const noexcept
{
        return {
                position_,
                width_,
                height_
        };
}

void Mike::move_in_direction(Engine::Gameplay::Direction direction) noexcept
{
        auto const push_or_pull =
        [this](Block* block, Engine::Gameplay::Direction direction)
        {
                if (direction_ == direction)
                        state_ = Pushing_block {block};
                else
                        state_ = Pulling_block {block};
        };
        
        Utils::lambda_visit(state_,
                [push_or_pull, direction](Holding_block holding_block) noexcept
                { push_or_pull(holding_block.block, direction); },

                [push_or_pull, direction](Pushing_block pushing_block) noexcept
                { push_or_pull(pushing_block.block, direction); },

                [push_or_pull, direction](Pulling_block pulling_block) noexcept
                { push_or_pull(pulling_block.block, direction); },

                [this, direction](auto const&) noexcept
                {
                        direction_ = direction;
                        state_ = Running();
                }
        );
}

void register_mike_keyboard_controls(Mike& mike, Blocks& blocks, Engine::Gameplay::Signals& signals)
{
        auto const user_has_control =
        [](Mike const& mike) noexcept {
                return mike.is_standing_still() ||
                       mike.is_running() ||
                       mike.is_holding_block() ||
                       mike.is_pushing_block() ||
                       mike.is_pulling_block();
        };

        auto const on_frame_advance =
        [&mike, &blocks, user_has_control](Engine::Gameplay::Main_loop& main_loop,
                                           Engine::Gameplay::Keyboard const& keyboard)
        {
                // TODO Ditch the `user_has_control` bullshit here and keep it internalized within `Mike`
                if (!user_has_control(mike))
                        return;

                if (keyboard.key_down(Engine::Sdl::Keycodes::e))
                        mike.hold_nearest_block(blocks);
                else
                        mike.stand_still();

                if (keyboard.key_down(Engine::Sdl::Keycodes::a) ||
                    keyboard.key_down(Engine::Sdl::Keycodes::left)) {
                        mike.move_left();
                }

                if (keyboard.key_down(Engine::Sdl::Keycodes::d) ||
                    keyboard.key_down(Engine::Sdl::Keycodes::right)) {
                        mike.move_right();
                }

                if (keyboard.key_down(Engine::Sdl::Keycodes::space) ||
                    keyboard.key_down(Engine::Sdl::Keycodes::up) ||
                    keyboard.key_down(Engine::Sdl::Keycodes::w)) {
                        main_loop.register_callback(mike.jump());
                } 
        };

        signals.frame_advance.connect(on_frame_advance);
}

}

