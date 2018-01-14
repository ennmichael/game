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

enum class Direction {
        none,
        right,
        left
};

Direction opposite_direction(Direction direction) noexcept;

struct Checkbox;

using Const_checkboxes_pointers = std::vector<Checkbox const*>;
using Checkboxes_pointers = std::vector<Checkbox*>;

Const_checkboxes_pointers close_checkboxes(Checkboxes_pointers const& checkboxes,
                                     Complex_number pivot,
                                     double minimum_distance);

struct Checkbox {
        Sdl::Rect to_rect() const noexcept;
        bool can_be_translated(Complex_number delta,
                               Const_checkboxes_pointers const& solid_checkboxes) const noexcept;
        bool can_be_translated(Engine::Gameplay::Direction direction,
                               double delta,
                               Const_checkboxes_pointers const& solid_checkboxes) const noexcept;
        bool collides_with(Checkbox checkbox) const noexcept;
        bool collides_with_any(Const_checkboxes_pointers const& checkboxes) const noexcept;
        Checkbox translated(Complex_number delta) const noexcept;
        Checkbox translated(Direction direction, double delta) const noexcept;

        Complex_number position;
        int width;
        int height;
};

bool operator==(Checkbox x, Checkbox y) noexcept;
bool operator!=(Checkbox x, Checkbox y) noexcept;

Engine::Complex_number center_position(Checkbox checkbox) noexcept;

template <class T>
Engine::Complex_number center_position(T const& obj) noexcept(noexcept(obj.checkbox()))
{
        return center_position(obj.checkbox());
}

Complex_number translated_position(Complex_number position,
                                   Direction direction,
                                   double delta) noexcept;

}

