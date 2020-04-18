#pragma once

namespace kgfx {

    //
    class Frame_time {
    public:
        Frame_time();

    public:
        void next_frame();

    public:
        unsigned long current_time_ms() const;
        unsigned long delta_time_ms() const;
        double delta_time_sec() const;
        double fps() const;

    public:
        static unsigned long time_diff(unsigned long first, unsigned long second);

    private: 
        unsigned long current_time_;
        unsigned long delta_time_;

        unsigned long frame_count_;
        unsigned long last_fps_sample_;
        double fps_;
    };

} // namespace kgfx
