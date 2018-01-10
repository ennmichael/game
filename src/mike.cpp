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

bool Mike::is_in_motion() const noexcept
{
        return is_jumping_sideways() || is_running();
}

bool Mike::is_facing_left() const noexcept
{
        return direction_ == Direction::left;
}

bool Mike::is_facing_right() const noexcept
{
        return direction_ == Direction::right;
}

void Mike::grab_nearest_block(Blocks& blocks) noexcept
{
        auto const closest_block =
        [this](Blocks& blocks) noexcept
        {
                return std::min_element(blocks.begin(), blocks.end(),
                        [this](Block b1, Block b2)
                        {
                                auto const d1 = Engine::Utils::distance(b1, *this);
                                auto const d2 = Engine::Utils::distance(b2, *this);
                                return d1 < d2;
                        }
                );
        };

        auto const block_is_close_enough =
        [this](Block block)
        {
                auto const distance = Engine::Utils::distance(Engine::Gameplay::center_position(block),
                                                              Engine::Gameplay::center_position(*this));

                return distance < width_ + block.width/2;
        };

        auto const block = closest_block(blocks);
        if (block != blocks.cend() && block_is_close_enough(*block))
                state_ = Holding_block {block};

}

void Mike::move_left() noexcept
{
        run_in_direction(Direction::left); 
}

void Mike::move_right() noexcept
{
        run_in_direction(Direction::right);
}

void Mike::stand_still() noexcept
{
        direction_ = Direction::none;
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

void Mike::update_position(Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) noexcept
{
        auto const move_forward =
        [this](Engine::Gameplay::Checkboxes_thunks solid_checkboxes_thunks)
        {
                if (is_facing_left())
                        translate_if_possible(-speed_, solid_checkboxes_thunks);
                else if (is_facing_right())
                        translate_if_possible(speed_, solid_checkboxes_thunks);
        };

        if (is_in_motion())
                move_forward(solid_checkboxes_thunks);
}

Direction Mike::direction() const noexcept
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

bool Mike::can_be_translated(Engine::Complex_number delta,
                             Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) const noexcept
{
        return checkbox().can_be_translated(delta, solid_checkboxes_thunks);
}

void Mike::translate_if_possible(Engine::Complex_number delta,
                                 Engine::Gameplay::Checkboxes_thunks const& solid_checkboxes_thunks) noexcept
{
        if (can_be_translated(delta, solid_checkboxes_thunks))
                position_ += delta;
}

void Mike::run_in_direction(Direction direction) noexcept
{
        Utils::lambda_visit(state_,
                [this, direction](Holding_block holding_block) noexcept
                {
                        if (direction_ == direction)
                                state_ = Pushing_block {holding_block.block};
                        else
                                state_ = Pulling_block {holding_block.block};
                },
                [this, direction](auto const&) noexcept
                {
                        direction_ = direction;
                        state_ = Running();
                }
        );
}

void register_mike_keyboard_controls(Mike& mike, Blocks& blocks, Engine::Gameplay::Signals& signals)
{
        struct Key_binding { // We could use function pointers here, if that matters
                std::function<bool(Engine::Gameplay::Keyboard const&)> checker;
                std::function<void(Engine::Gameplay::Main_loop&)> callback;
        };

        std::vector key_bindings {
                Key_binding {
                        [](Engine::Gameplay::Keyboard const& keyboard) noexcept
                        {
                                return keyboard.key_down(Engine::Sdl::Keycodes::a) ||
                                       keyboard.key_down(Engine::Sdl::Keycodes::left);
                        },
                        [&](Engine::Gameplay::Main_loop&)
                        {
                                mike.move_left();
                        }
                },
                Key_binding {
                        [](Engine::Gameplay::Keyboard const& keyboard) noexcept
                        {
                                return keyboard.key_down(Engine::Sdl::Keycodes::d) ||
                                       keyboard.key_down(Engine::Sdl::Keycodes::right);
                        },
                        [&](Engine::Gameplay::Main_loop&)
                        {
                                mike.move_right();
                        }
                },
                Key_binding {
                        [](Engine::Gameplay::Keyboard const& keyboard) noexcept
                        {
                                return keyboard.key_pressed(Engine::Sdl::Keycodes::w) ||
                                       keyboard.key_pressed(Engine::Sdl::Keycodes::up) ||
                                       keyboard.key_pressed(Engine::Sdl::Keycodes::space);
                        },
                        [&](Engine::Gameplay::Main_loop& main_loop)
                        {
                                main_loop.register_callback(mike.jump());
                        }
                },
                Key_binding {
                        [](Engine::Gameplay::Keyboard const& keyboard) noexcept
                        {
                                return keyboard.key_pressed(Engine::Sdl::Keycodes::e);
                        },
                        [&](Engine::Gameplay::Main_loop&)
                        {
                                mike.grab_nearest_block(blocks);
                        }
                }
        };

        auto const on_frame_advance =
        [&mike, key_bindings](Engine::Gameplay::Main_loop& main_loop,
                              Engine::Gameplay::Keyboard const& keyboard)
        {
                // TODO Ditch the `user_has_control` bullshit here and keep it internalized within `Mike`

                auto const receiving_input =
                [](std::vector<Key_binding> const& key_bindings,
                   Engine::Gameplay::Keyboard const& keyboard)
                {
                        return std::any_of(key_bindings.cbegin(), key_bindings.cend(),
                                [&](Key_binding key_binding)
                                {
                                        return key_binding.checker(keyboard);
                                }
                        );
                };

                auto const dispatch_key_binding =
                [](Key_binding const& key_binding,
                   Engine::Gameplay::Keyboard const& keyboard,
                   Engine::Gameplay::Main_loop& main_loop)
                {
                        if (key_binding.checker(keyboard))
                                key_binding.callback(main_loop);
                };

                if (!user_has_control(mike))
                        return;

                if (receiving_input(key_bindings, keyboard)) {
                        for (auto const& key_binding : key_bindings)
                                dispatch_key_binding(key_binding, keyboard, main_loop);
                } else {
                        mike.stand_still();
                }
        };

        signals.frame_advance.connect(on_frame_advance);
}

bool user_has_control(Mike const& mike)
{
        return mike.is_standing_still() ||
               mike.is_running() ||
               mike.is_holding_block() ||
               mike.is_pushing_block() ||
               mike.is_pulling_block();
}

}

