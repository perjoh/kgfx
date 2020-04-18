#pragma once

namespace kgfx {

    class Event_handler
    {
    public : 
        Event_handler() = default;
        Event_handler(const Event_handler&) = delete;

    public :
        bool poll_events();

    private :
    };

} // namespace kgfx
