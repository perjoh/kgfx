#pragma once

namespace kgfx 
{

	void create_window(unsigned window_width, 
                       unsigned window_height, 
                       const char* window_title); 

	void destroy_window();

	void begin_render();
	void end_render();

	bool handle_events();

} // namespace kgfx
