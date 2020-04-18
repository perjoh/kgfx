#include <kgfx/window.hpp>
#include <SDL.h>
#include <GL/glew.h>
#include <stdexcept>
#include <cassert>

namespace kgfx {

	SDL_Window* g_window = nullptr; 
	SDL_GLContext g_glcontext = nullptr; 

    //
	void create_window(	unsigned window_width, 
					    unsigned window_height, 
						const char* window_title) 
	{ 
		assert(g_window == nullptr);

		g_window = ::SDL_CreateWindow(	window_title,
										SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
										window_width, window_height,
										SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

		if (nullptr == g_window) 
		{
			throw std::runtime_error("Failed to create window.");
		}

		g_glcontext = ::SDL_GL_CreateContext(g_window);
		if (nullptr == g_glcontext) 
		{
			throw std::runtime_error("Failed to create OpenGL context.");
		}

		GLenum error = ::glewInit();
		if (error != GLEW_OK)
		{
			throw std::runtime_error("glewInit failed.");
		} 

		::glEnable(GL_DEPTH_TEST);

		::glEnable(GL_CULL_FACE);
		::glFrontFace(GL_CW);
		::glCullFace(GL_BACK);

		// Enable v-sync.
		::SDL_GL_SetSwapInterval(1);
	}

    //
	void destroy_window() 
	{ 
		if (g_window) 
		{
			::SDL_DestroyWindow(g_window);
		}
	}


	void clear_buffers()
	{
        glClearColor(0.0, 0.0, 0.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}


	void begin_render()
	{
		clear_buffers();
	}


	void end_render()
	{
		::SDL_GL_SwapWindow(g_window);
	}


	bool handle_events() 
	{ 
		::SDL_Event e;
		while (::SDL_PollEvent(&e)) 
		{ 
			switch (e.type) 
			{ 
				case SDL_KEYUP:
					if (e.key.keysym.sym == SDLK_ESCAPE)
						return false;
					break;
				case SDL_QUIT:
					return false;
			}
		} 

		return true; 
	} 

} // namespace kgfx
