#include "gameplay.h"
#include "utils.h"
#include <unordered_map>
#include <optional>
#include <exception>
#include <cassert>
#include <type_traits>
#include <iterator>
#include <algorithm>






#include <iostream> // TODO Remove me

namespace Engine::Gameplay {

Timer::Timer(Duration::Milliseconds duration) noexcept
        : duration_(duration)
{}

bool Timer::ready() const noexcept
{
        auto const ticks = Sdl::get_ticks();
        return ticks >= initiation_time_ + duration_;
}

void Timer::restart() noexcept
{
        *this = restarted();
}

Timer Timer::restarted() const noexcept
{
        return Timer(duration_);
}

Frame_timer::Frame_timer(Duration::Frames duration) noexcept
        : duration_(duration)
{}

void Frame_timer::update() noexcept
{
        if (Utils::underlying_value(remaining_duration_) > 0)
                --remaining_duration_;
}

bool Frame_timer::ready() const noexcept
{
        return Utils::underlying_value(remaining_duration_) <= 0;
}

void Frame_timer::restart() noexcept
{
        *this = restarted();
}

Frame_timer Frame_timer::restarted() const noexcept
{
        return Frame_timer(duration_);
}

bool Timed_callback::ready() const noexcept
{
        return timer_.ready();
}

void Timed_callback::update_timer() noexcept
{
        timer_.update();
}

std::optional<Timed_callback> Timed_callback::execute()
{
        struct Visitor {
                std::optional<Timed_callback> operator()(std::function<void()> f) const
                {
                        f();
                        return std::nullopt;
                }

                std::optional<Timed_callback> operator()(std::function<Timed_callback()> f) const
                {
                        return f();
                }
        };

        return std::visit(Visitor(), function_);
}

void update_callbacks_timers(Timed_callbacks& callbacks) noexcept
{
        for (auto& cb : callbacks)
                cb.update_timer();
}

void execute_ready_callbacks(Timed_callbacks& callbacks)
{
        auto const ready_callbacks =
        [](Timed_callbacks& callbacks)
        {
                std::vector<Timed_callbacks::iterator> result;

                for (auto i = callbacks.begin(); i != callbacks.end(); ++i) {
                        if (i->ready())
                                result.push_back(i);
                }

                return result;
        };

        Timed_callbacks new_callbacks;

        for (auto const cb : ready_callbacks(callbacks)) {
                if (auto const new_callback = cb->execute())
                        new_callbacks.push_back(*new_callback);
                callbacks.erase(cb);
        }

        std::copy(new_callbacks.cbegin(), new_callbacks.cend(), 
                  std::back_inserter(callbacks));
}

void Main_loop::start(Signals const& signals, int fps)
{
        auto const calculate_frame_delay =
        [](int fps) noexcept
        {
                auto const frames_per_millisecond = fps / 1000.0;
                return Duration::Milliseconds {
                        static_cast<std::underlying_type_t<Duration::Milliseconds>>(1 / frames_per_millisecond)
                };
        };

        auto const dispatch_event =
        [](Sdl::Event const& event, Keyboard& keyboard, Signals const& signals)
        {
                keyboard.update(event);

                if (event.type == SDL_QUIT) {
                        signals.quit();
                        return true;
                }

                return false;
        };

        auto const frame_delay = calculate_frame_delay(fps);
        Timer timer(frame_delay);
        Keyboard keyboard;

        auto quit = false;
        while (!quit) {
                while (auto const event = Sdl::poll_event())
                        quit = dispatch_event(*event, keyboard, signals);

                if (timer.ready()) {
                        update_callbacks_timers(callbacks_);
                        execute_ready_callbacks(callbacks_);
                        signals.frame_advance(*this, keyboard); 
                        timer.restart();
                }
        }
}

Direction opposite_direction(Direction direction) noexcept
{
        switch (direction) {
                case Direction::none:  return Direction::none;
                case Direction::right: return Direction::left;
                case Direction::left:  return Direction::right;
        }

        assert(false);
        return Direction::none;
}

void Main_loop::register_callback(Timed_callback const& callback)
{
        callbacks_.push_back(callback);
}

Const_checkboxes_pointers close_checkboxes(Const_checkboxes_pointers const& checkboxes,
                                     Complex_number pivot,
                                     double minimum_distance)
{
        return Utils::filter(checkboxes,
                [pivot, minimum_distance](Checkbox const* checkbox)
                {
                        auto const d = distance(*checkbox, pivot);
                        return d <= minimum_distance;
                }
        );
}

Sdl::Rect Checkbox::to_rect() const noexcept
{
        return {
                static_cast<int>(position.real()),
                static_cast<int>(position.imag()),
                width,
                height
        };
}

bool Checkbox::can_be_translated(Complex_number delta,
                                 Const_checkboxes_pointers const& solid_checkboxes) const noexcept
{
        auto const translated_checkbox = translated(delta);
        return !translated_checkbox.collides_with_any(solid_checkboxes);
}

bool Checkbox::can_be_translated(Direction direction,
                                 double delta,
                                 Const_checkboxes_pointers const& solid_checkboxes) const noexcept
{
        auto const translated_checkbox = translated(direction, delta);
        return !translated_checkbox.collides_with_any(solid_checkboxes);
}

bool Checkbox::collides_with(Checkbox checkbox) const noexcept
{
        return Sdl::has_intersection(to_rect(), checkbox.to_rect());
}

bool Checkbox::collides_with_any(Const_checkboxes_pointers const& checkboxes) const noexcept
{
        return std::any_of(checkboxes.cbegin(), checkboxes.cend(),
                [this](Checkbox const* checkbox) noexcept
                {
                        std::cout << (checkbox == this) << '\n';
                        std::cout << collides_with(*checkbox) << '\n';
                        if (checkbox == this)
                                return false;
                        return collides_with(*checkbox);
                }
        );
}

Checkbox Checkbox::translated(Complex_number delta) const noexcept
{
        return {
                position + delta,
                width,
                height
        };
}

Checkbox Checkbox::translated(Direction direction, double delta) const noexcept
{
        return {
                translated_position(position, direction, delta),
                width,
                height
        };
}

bool operator==(Checkbox x, Checkbox y) noexcept
{
        return x.position == y.position &&
               x.width == y.width &&
               x.height == y.height;
}

bool operator!=(Checkbox x, Checkbox y) noexcept
{
        return !(x == y);
}

Complex_number center_position(Checkbox checkbox) noexcept
{
        return checkbox.position + Complex_number(checkbox.width / 2,
                                                  checkbox.height / 2);
}

Complex_number translated_position(Complex_number position,
                                   Direction direction,
                                   double delta) noexcept
{
        switch (direction) {
                case Direction::none:  return position;
                case Direction::right: return position + delta;
                case Direction::left:  return position - delta;
        }

        assert(false);
        return Complex_number();
}

}

