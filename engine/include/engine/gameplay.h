#pragma once

#include "boost/signals2.hpp"
#include "base.h"
#include "sdl++.h"
#include "keyboard.h"
#include <memory>
#include <functional>
#include <unordered_map>
#include <utility>
#include <functional>
#include <variant>
#include <optional>
#include <type_traits>

namespace Engine::Gameplay {

class Main_loop;

class Timer {
public:
        explicit Timer(Duration::Milliseconds duration) noexcept;

        bool ready() const noexcept;
        void restart() noexcept;
        Timer restarted() const noexcept;

private:
        Duration::Milliseconds duration_;
        Duration::Milliseconds initiation_time_ = Sdl::get_ticks();
};

class Frame_timer {
public:
        explicit Frame_timer(Duration::Frames duration) noexcept;

        void update() noexcept;
        bool ready() const noexcept;

        void restart() noexcept;
        Frame_timer restarted() const noexcept;

private:
        Duration::Frames duration_;
        Duration::Frames remaining_duration_ = duration_;
};

class Timed_callback {
public:
        using Function = std::variant<std::function<void()>, std::function<Timed_callback()>>;

        template <class F>
        Timed_callback(F f, Duration::Frames duration)
                : function_(std::function<std::invoke_result_t<F>()>(f))
                , timer_(duration)
        {}

        bool ready() const noexcept;
        void update_timer() noexcept;
        std::optional<Timed_callback> execute();

private:
        Function function_;
        Frame_timer timer_;
};

using Timed_callbacks = std::vector<Timed_callback>;

void update_callbacks_timers(Timed_callbacks& callbacks) noexcept;
void execute_ready_callbacks(Timed_callbacks& callbacks);

class Main_loop;

struct Signals {
        boost::signals2::signal<void(Main_loop&, Keyboard const&)> frame_advance;
        boost::signals2::signal<void()> quit;
};

class Main_loop {
public:
        void start(Signals const& signals, int fps);
        void register_callback(Timed_callback const& callback);

private:
        std::vector<Timed_callback> callbacks_;
};

struct Checkbox;

using Checkbox_thunk = std::function<Checkbox()>;
using Checkboxes_thunks = std::vector<Checkbox_thunk>;

Checkboxes_thunks close_checkboxes(Checkboxes_thunks const& checkboxes,
                                   Complex_number pivot,
                                   double minimum_distance);

struct Checkbox {
        Sdl::Rect to_rect() const noexcept;
        bool can_be_translated(Complex_number delta,
                               Checkboxes_thunks const& solid_checkboxes) const noexcept;
        bool collides_with(Checkbox checkbox) const noexcept;
        bool collides_with_any(Checkboxes_thunks const& checkboxes) const noexcept;
        Checkbox translated(Complex_number delta) const noexcept;

        Complex_number position;
        int width;
        int height;
};

}

