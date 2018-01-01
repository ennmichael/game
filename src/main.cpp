#include "engine/engine.h"
#include <string>



#include <iostream>






using namespace std::string_literals;
using namespace std::complex_literals;

namespace Game::Logic {

enum class Direction {
        left, right
};

struct Object_position { // I plan for every object (i.e. the main character, 
                         // the enemies, the whatever) to have a coordinate and a velocity,
        void update()    // so I group the two in this struct
        {
                coordinates += velocity;
        }

        Engine::Complex_number coordinates = 0.0 + 0.0i;
        Engine::Complex_number velocity = 0.0 + 0.0i;
};

class Lightbulb_man {
public: // TODO This will probably also take an Engine::Graphics::Dimensions parameter for the checkbox
        explicit Lightbulb_man(Engine::Complex_number coordinates)
                : position_{coordinates}
        {
                auto const config = Engine::Config::load_from_file("../res/lightbulb-man.config"s);
                config.unpack_value("speed", speed_);
        }

        void move_left() noexcept
        {
                direction_ = Direction::left;
                position_.velocity_ = -speed_;
        }

        void move_right() noexcept
        {
                direction_ = Direction::right;
                position_.velocity_ = speed_;
        }

        void stay_still() noexcept
        {
                position_.velocity = 0.0 + 0.0i;
        }

        void update_position() noexcept
        {
                position_.update();
        }

        Engine::Complex_number position() const noexcept
        {
                return position_;
        }

        Direction direction() const noexcept
        {
                return direction_;
        }

        void register_movement(Engine::Gameplay::Signals& signals)
        {
                auto const move_left_cb =
                [this]
                {
                        move_left();
                };

                auto const move_right_cb =
                [this]
                {
                        move_right();
                };

                signals.a_pressed(move_left_cb);
                signals.left_pressed(move_left_cb);

                signals.d_pressed(move_right_cb);
                signals.right_pressed(move_right_cb);
        }

private:
        Object_position position;
        double speed_ = 0;
        Direction direction_ = Direction::left;
};

// TODO Fix namespacing to Engine::Sdl

void draw_player(Sdl::Renderer const& renderer, 
                 Lightbulb_man const& lightbulb_man,
                 Engine::Sprite const& sprite)
{
        auto const flip = (lightbulb_man.direction() == Direction::left) ?
                Sdl::Flip::horizontal : Sdl::Flip::none

        sprite.render(renderer, lightbulb_man.position(), 0, flip);
}

}

int main()
{
        Sdl::Manager manager;
        (void)manager;

        auto const [window, renderer] = Sdl::create_window_and_renderer("Title"s, 500, 500);
        (void)window;

        auto spr = Engine::Graphics::Sprite::load_from_file(renderer, "../res/sprites/run"s);
        auto animation = spr.animation();

        Lightbulb_man lightbulb_man(0.0 + 150.0i);

        auto const on_frame_advance =
        [&]()
        {
                Sdl::render_clear(renderer);
                animation.frame_advance();
                spr.render(renderer, position);
                Sdl::render_present(renderer);
        };

        Engine::Gameplay::Signals signals;


        signals.frame_advance.connect(on_frame_advance);

        Engine::Gameplay::main_loop(signals, 60);
}

