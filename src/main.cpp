#include "engine/engine.h"
#include <string>



#include <iostream>






using namespace std::string_literals;
using namespace std::complex_literals;

namespace Game::Logic {

enum class Faced_direction {
        left, right
};

struct Entity_position {
        void update()
        {
                coordinates += velocity;
        }

        Engine::Complex_number coordinates = 0.0 + 0.0i;
        Engine::Complex_number velocity = 0.0 + 0.0i;
};

class Lightbulb_man {
public:
        explicit Lightbulb_man(Engine::Complex_number coordinates)
                : position_{coordinates}
        {
                auto const config = Engine::Config::load_from_file("../res/lightbulb-man.config"s);
                config.unpack_value("speed", speed_);
        }

        void move_left() noexcept
        {
                direction_ = -speed_;
        }

        void move_right() noexcept
        {
                direction_ = speed_;
        }

        void stand_still() noexcept
        {
                direction_ = 0.0;
        }

        void update_position() noexcept
        {
                position_ += direction_;
        }

        bool standing_still() const noexcept
        {
                return direction_.real() == 0;
        }

        bool facing_right() const noexcept
        {
                return direction_.real() > 0;
        }

        bool facing_left() const noexcept
        {
                return direction_.real() < 0;
        }

        Engine::Complex_number position() const noexcept
        {
                return position_;
        }

        void register_keyboard_controls(Engine::Gameplay::Signals& signals)
        {
                signals.keyboard_change.connect(
                        [this](Engine::Gameplay::Keyboard const& keyboard)
                        {
                                if (keyboard.key_down(Sdl::Keycodes::a) || 
                                    keyboard.key_down(Sdl::Keycodes::left)) {
                                        move_left();
                                } else if (keyboard.key_down(Sdl::Keycodes::d) ||
                                           keyboard.key_down(Sdl::Keycodes::right)) {
                                        move_right();
                                } else {
                                        stand_still();
                                }
                        }
                );
        }

private:
        Engine::Complex_number position_;
        Engine::Complex_number direction_ = 0.0 + 0.0i;
        double speed_ = 0;
};

class Lightbulb_man_graphics {
public:
        explicit Lightbulb_man_graphics(Sdl::Renderer const& renderer)
                : idle_sprite_(Engine::Graphics::Sprite::load_from_file(renderer,
                                                                        "../res/sprites/idle"s))
                , running_sprite_(Engine::Graphics::Sprite::load_from_file(renderer,
                                                                           "../res/sprites/run"s))
        {
        }

        void frame_advance()
        {
                idle_animation_.frame_advance();
                running_animation_.frame_advance();
        }

        void render(Sdl::Renderer& renderer, Lightbulb_man const& lightbulb_man)
        {
                auto& sprite = present_sprite(lightbulb_man);
                auto const flip = present_flip(lightbulb_man);
                sprite.render(renderer, lightbulb_man.position(), 0, flip);
        }

private:
        Engine::Graphics::Sprite& present_sprite(Lightbulb_man const& lightbulb_man) noexcept
        {
                if (lightbulb_man.standing_still())
                        return idle_sprite_;
                return running_sprite_;
        }

        Sdl::Flip present_flip(Lightbulb_man const& lightbulb_man)
        {
                if (lightbulb_man.facing_left())
                        return Sdl::Flip::horizontal;
                return Sdl::Flip::none;
        }

        Engine::Graphics::Sprite idle_sprite_;
        Engine::Graphics::Animation idle_animation_ = idle_sprite_.animation();

        Engine::Graphics::Sprite running_sprite_;
        Engine::Graphics::Animation running_animation_ = running_sprite_.animation();
};

}

// TODO Engine::Graphics::Sdl

int main()
{
        Sdl::Manager manager;
        (void)manager;

        // Due to working with `unique_ptr` it turns out I have const correctness issues
        auto [window, renderer] = Sdl::create_window_and_renderer("Title"s, 500, 500);
        (void)window;

        // Clearly shouldn't be in `Game::Logic`
        Game::Logic::Lightbulb_man_graphics lightbulb_man_graphics(renderer);

        Game::Logic::Lightbulb_man lightbulb_man(0.0 + 150.0i);
        Engine::Gameplay::Signals signals;

        lightbulb_man.register_keyboard_controls(signals);

        auto const on_frame_advance =
        [&]
        {
                lightbulb_man.update_position();

                Sdl::render_clear(renderer);
                lightbulb_man_graphics.frame_advance();
                lightbulb_man_graphics.render(renderer, lightbulb_man);
                Sdl::render_present(renderer);
        };

        signals.frame_advance.connect(on_frame_advance);
        Engine::Gameplay::main_loop(signals, 60);
}

