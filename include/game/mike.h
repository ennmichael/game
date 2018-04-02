#pragma once

#include "engine/engine.h"
#include "block.h"
#include "utils.h"
#include "boost/optional.hpp"
#include "boost/property_tree/ptree.hpp"
#include <unordered_map>
#include <functional>

namespace Game::Gameplay {

class Mike {
public:
        using Actions_durations = std::unordered_map<std::string, Engine::Duration::Frames>;

        Mike(boost::property_tree::ptree const& config,
             Engine::Graphics::Animations animations);

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

        void render(Engine::Sdl::Renderer& renderer);
        void render_sickness_filter(Engine::Sdl::Renderer& renderer);

private:
        struct State;
        using Optional_state = boost::optional<State>; 

        struct State {
                using On_update = std::function<Optional_state(Mike&, Engine::Gameplay::Keyboard const&)>;
                using On_set = std::function<void(Mike&)>;
                using On_unset = std::function<void(Mike&)>;

                using On_update_function_pointer = Optional_state (*)(Mike&, Engine::Gameplay::Keyboard const&);
                using On_set_function_pointer = void (*)(Mike&);
                using On_unset_function_pointer = void (*)(Mike&);

                static constexpr On_update_function_pointer on_update_do_nothing()
                {
                        return [](Mike&, Engine::Gameplay::Keyboard const&) -> Optional_state
                               { return boost::none; };
                } 

                static constexpr On_set_function_pointer on_set_do_nothing()
                { return [](Mike&) {}; }

                static constexpr On_unset_function_pointer on_unset_do_nothing()
                { return [](Mike&) {}; }

                On_update on_update=on_update_do_nothing();
                On_set on_set=on_set_do_nothing();
                On_unset on_unset=on_unset_do_nothing();
        };

        class Sickness {
        public:
                Sickness(double increase_rate, double decrease_rate) noexcept;

                double percentage() const noexcept;
                void increase() noexcept;
                void decrease() noexcept;

        private:
                double increase_rate_;
                double decrease_rate_;
                double percentage_=0;
        };

        static State idle();
        static State running();
        static State idle_masked();
        static State walking_masked();
        static State jumping_in_place();
        static State preparing_to_jump_sideways();
        static State jumping_sideways();
        static State landing_sideways();

        static State single_pass_animation(char const* animation_name, State next_state);
        static State looping_animation(char const* animation_name);

        template <class OnUpdate, class NextState>
        static State single_pass_animation(char const* animation_name,
                                           OnUpdate&& on_update,
                                           NextState&& next_state)
        {
                return {
                        [on_update  = std::forward<OnUpdate>(on_update),
                         next_state = std::forward<NextState>(next_state)
                        ](Mike& mike, Engine::Gameplay::Keyboard const& keyboard) -> Optional_state
                        {
                                mike.current_animation_.update();
                                if (mike.current_animation_.is_finished())
                                        return next_state;

                                return on_update(mike, keyboard);
                        },

                        [animation_name](Mike& mike)
                        { mike.hard_switch_animation(animation_name); } 
                };
        }

        template <class OnUpdate>
        static State looping_animation(char const* animation_name,
                                       OnUpdate on_update)
        {
                return {
                        [on_update = std::forward<OnUpdate>(on_update)]
                        (Mike& mike, Engine::Gameplay::Keyboard const& keyboard) -> Optional_state
                        {
                                mike.current_animation_.update();
                                if (mike.current_animation_.is_finished())
                                        mike.current_animation_.loop();

                                return on_update(mike, keyboard);
                        },
                
                        [animation_name](Mike& mike)
                        { mike.hard_switch_animation(animation_name); }
                };
        }

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

        void hard_switch_animation(std::string const& animation_name);
        void soft_switch_animation(std::string const& animation_name);

        Engine::Sdl::Flip current_flip() const noexcept;

        Engine::Complex_number position_;
        Engine::Dimensions dimensions_;
        Sickness sickness_;
        double speed_;
        double jump_speed_;
        double masked_speed_;
        Engine::Graphics::Animations animations_;
        Engine::Gameplay::Direction direction_ = Engine::Gameplay::Direction::none;
        State state_=idle();
        Engine::Graphics::Current_animation current_animation_ = Engine::Graphics::Current_animation(
                                                                                animations_.at("idle"));
};

}

