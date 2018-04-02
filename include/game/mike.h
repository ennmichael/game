#pragma once

#include "engine/engine.h"
#include "block.h"
#include "utils.h"
#include "configs.h"
#include "boost/optional.hpp"
#include "boost/property_tree/ptree.hpp"
#include <unordered_map>
#include <functional>

namespace Game::Gameplay {

// TODO Refactor hold_nearest_block -> grab_nearest_block and make sure it doesn't do redundant work
// TODO Add a `interacting_with_block` member function
// TODO Use `idle` to make sure that Mike keeps holding his block
// TODO Add a `release_held_block` member function

class Mike {
public:
        using Actions_durations = std::unordered_map<std::string, Engine::Duration::Frames>;

        Mike(boost::property_tree::ptree const& config,
             Actions_durations const& actions_durations);

        bool is_facing_left() const noexcept;
        bool is_facing_right() const noexcept;

        template <class T>
        void snap_to(T const& obj) noexcept(noexcept(obj.checkbox()))
        {
                snap_to(obj.checkbox());
        }

        void snap_to(Engine::Gameplay::Checkbox checkbox) noexcept;
        void update(Engine::Gameplay::Keyboard const& keyboard);
      
        Engine::Gameplay::Direction direction() const noexcept;
        Engine::Complex_number position() const noexcept;
        Engine::Gameplay::Checkbox checkbox() const noexcept;
        int sickness_percentage() const noexcept;

        std::string const& current_sprite_name() const noexcept;

private:
        struct State;
        using Optional_state = boost::optional<State>;

        class Sickness {
        public:
                Sickness(int increase_rate, int decrease_rate) noexcept;

                int percentage() const noexcept;
                void increase() noexcept;
                void decrease() noexcept;

        private:
                int percentage_;
                int increase_rate_;
                int decrease_rate_;
        };

        struct State {
                using Update = std::function<Optional_state(Mike&, Engine::Gameplay::Keyboard const&)>;

                std::string sprite_name;
                Update update=[](Mike&, Engine::Gameplay::Keyboard const&) -> Optional_state
                              { return boost::none; };
        };

        static State idle();
        static State running();
        static State idle_masked();
        static State walking_masked();
        static State jumping_in_place(Actions_durations const& durations);
        static State preparing_to_jump_sideways(Actions_durations const& durations);
        static State jumping_sideways(Actions_durations const& durations);
        static State landing_sideways(Actions_durations const& durations);

        static State expiring_state(State state, State next_state, Actions_durations const& durations);

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

        void move_left() noexcept;
        void move_right() noexcept;
        void move_in_current_direction(double speed) noexcept;
        void move_in_direction(Engine::Gameplay::Direction direction) noexcept;
        void move_in_direction(Engine::Gameplay::Direction direction,
                               double speed) noexcept;

        Engine::Complex_number position_;
        Engine::Dimensions dimensions_;
        Sickness sickness_;
        double speed_;
        double jump_speed_;
        double masked_speed_;
        Actions_durations durations_; // TODO Rename durations_ -> actions_durations_
        Engine::Gameplay::Direction direction_ = Engine::Gameplay::Direction::none;
        State state_=idle();
};

void apply_sickness_filter(Sdl::Renderer& renderer, Mike const& mike);

}

