#include <kgfx/event_handler.hpp>
#include <SDL.h>

namespace kgfx {

    bool Event_handler::poll_events()
    {
		::SDL_Event e;
		while (::SDL_PollEvent(&e)) 
		{ 
			switch (e.type) 
			{ 
				case SDL_KEYUP: {
					if (e.key.keysym.sym == SDLK_ESCAPE) {
						return false;
                    }
                } break;

				case SDL_QUIT: {
					return false;
                } break;
			}
		} 

		return true; 
    }

} // namespace kgfx
