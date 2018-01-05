#include "gameplay.h"
#include "utils.h"
#include <unordered_map>
#include <optional>
#include <exception>
#include <cassert>
#include <type_traits>

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

void Main_loop::register_callback(Timed_callback const& callback)
{
        callbacks_.push_back(callback);
}

}

