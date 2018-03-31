#pragma once

#include "engine/engine.h"
#include "block.h"
#include "utils.h"
#include "boost/optional.hpp"
#include <unordered_map>
#include <functional>

namespace Game::Logic {

// TODO Refactor hold_nearest_block -> grab_nearest_block and make sure it doesn't do redundant work
// TODO Add a `interacting_with_block` member function
// TODO Use `idle` to make sure that Mike keeps holding his block
// TODO Add a `release_held_block` member function

class Mike {
public:
        struct State;
        using Optional_state = boost::optional<State>;

        struct State {
                using Updater = std::function<
                        Optional_state(Mike&, Engine::Gameplay::Keyboard const&)
                >;

                Updater updater;
                std::string sprite_name;
        };

        static State idle();
        static State running();

        using Actions_durations = std::unordered_map<std::string, Engine::Duration::Frames>;

        Mike(Engine::Complex_number position,
             Actions_durations const& durations,
             Engine::Sdl::Dimensions dimensions) noexcept;

        bool is_facing_left() const noexcept;
        bool is_facing_right() const noexcept;

        template <class T>
        void snap_to(T const& obj) noexcept(noexcept(obj.checkbox()))
        {
                snap_to(obj.checkbox());
        }

        void snap_to(Engine::Gameplay::Checkbox checkbox) noexcept;

        void move_left() noexcept;
        void move_right() noexcept;
      
        Engine::Gameplay::Direction direction() const noexcept;
        Engine::Complex_number position() const noexcept;
        Engine::Gameplay::Checkbox checkbox() const noexcept;

        std::string const& current_sprite_name() const noexcept;

private:
        template <class Entity>
        void turn_to(Entity const& entity) noexcept(noexcept(Engine::position(entity)))
        {
                if (Engine::position(entity).real() < position_.real())
                        direction_ = Engine::Gameplay::Direction::left;
                else if (Engine::position(entity).real() > position_.real())
                        direction_ = Engine::Gameplay::Direction::right;
                else
                        direction_ = Engine::Gameplay::Direction::none;
        }

        void move_in_direction(Engine::Gameplay::Direction direction) noexcept;

        Engine::Complex_number position_;
        Actions_durations durations_;
        Engine::Sdl::Dimensions dimensions_;
        Engine::Gameplay::Direction direction_ = Engine::Gameplay::Direction::none;
        State state_ = idle();
        double speed_ = 0;
};

}

