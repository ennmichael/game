#include "gameplay.h"
#include "utils.h"
#include <unordered_map>
#include <optional>
#include <exception>
#include <cassert>

namespace Engine::Gameplay {

Timer::Timer(Duration::Milliseconds duration) noexcept
        : duration_(duration)
{}

bool Timer::ready() const noexcept
{
        auto const ticks = Sdl::get_ticks();
        return ticks >= initiation_time_ + Utils::underlying_value(duration_);
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
        if (Utils::underlying_value(remaining_duration_) > 0) {
                remaining_duration_ = Duration::Frames {
                        Utils::underlying_value(remaining_duration_) - 1
                };
        }
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

Timed_callback::Timed_callback(Function function, Duration::Frames duration) noexcept
        : function_(function)
        , timer_(duration)
{}

bool Timed_callback::ready() const noexcept
{
        return timer_.ready();
}

void Timed_callback::update_timer() noexcept
{
        timer_.update();
}

void Timed_callback::execute()
{
        function_();
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

        for (auto const cb : ready_callbacks(callbacks)) {
                cb->execute();
                callbacks.erase(cb);
        }
}

void Main_loop::start(Signals const& signals, int fps)
{
        auto const calculate_frame_delay =
        [](int fps) noexcept
        {
                auto const frames_per_millisecond = fps / 1000.0;
                return Duration::Milliseconds {
                        static_cast<int>(1 / frames_per_millisecond)
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

                execute_ready_callbacks(callbacks_);

                if (timer.ready()) {
                        signals.frame_advance(*this, keyboard);
                        update_callbacks_timers(callbacks_);
                        timer.restart();
                }
        }
}

void Main_loop::register_callback(Timed_callback const& callback)
{
        callbacks_.push_back(callback);
}

}

