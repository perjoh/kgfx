#include <kgfx/frame_time.hpp>
#include <SDL.h>

namespace kgfx {

    unsigned long Frame_time::current_time_ms() const
    {
        return current_time_;
    }

    unsigned long Frame_time::delta_time_ms() const
    {
        return delta_time_;
    }

    double Frame_time::delta_time_sec() const
    {
        // If v-sync, return 1000/60.
        return static_cast<double>(delta_time_) / 1000.0;
    }

    double Frame_time::fps() const
    {
        return fps_;
    }

    void Frame_time::next_frame()
    {
        frame_count_ += 1;

        const auto prev_time = current_time_;
        current_time_ = ::SDL_GetTicks();
        delta_time_ = time_diff(prev_time, current_time_);

        const unsigned long max_frame_time = 1000 / 30;
        if (delta_time_ > max_frame_time)
        {
            delta_time_ = max_frame_time;
        }

        const unsigned long fps_sample_rate = 30;
        if (frame_count_ % fps_sample_rate == 0)
        {
            const auto prev_sample = last_fps_sample_;
            last_fps_sample_ = current_time_;

            fps_ = static_cast<double>(fps_sample_rate)
                   / static_cast<double>(time_diff(prev_sample, last_fps_sample_))
                   * 1000.0;

            frame_count_ = 0;
        }
    }

    unsigned long Frame_time::time_diff(unsigned long first, unsigned long second)
    {
        if (second < first)
        {
            return ~static_cast<unsigned long>(0) - first + second;
        }

        return second - first;
    }

    Frame_time::Frame_time()
        : current_time_(::SDL_GetTicks())
        , delta_time_(1)
        , frame_count_(0)
        , last_fps_sample_(current_time_)
        , fps_(0.0)
    {
    }

} // namespace kgfx
