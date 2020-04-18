#pragma once
#include "mesh.hpp"
#include "shader.hpp"
#include "../frame_time.hpp"
#include <klib/fast_delegate.hpp>
#include <SDL.h>
#include <memory>

namespace kgfx {

    //
    class Render_system
    {
    public :
        virtual ~Render_system() = default;

    public :
        virtual void render() = 0;
        virtual void update(Frame_time&) = 0;
    }; 

namespace opengl {

    class Mesh;

    //
    class Renderer 
    {
    public :
        Renderer() = default;
        Renderer(const Renderer&) = delete;

        ~Renderer();

    public :
        void construct_windowed(unsigned window_width,
                                unsigned window_height,
                                const char* window_title);

    public :
		/*using Render_callback = klib::Fast_delegate<void>;
        void on_render(Render_callback);

        using Update_callback = klib::Fast_delegate<void, const Frame_time&>;
        void on_update(Update_callback);*/

    public :
        void add_render_system(Render_system*);
        void remove_render_system(Render_system*);

    public :
        void run_frame();

    private :
        void render();
        void update();
        void present();

    public: 
        //Mesh allocate_mesh(const Triangle_mesh<>&);

    public :
        // Shader scripts must be named as 'name_vs.glsl' or 'name_fs.glsl'.
        // Examples:
        // To load basic_vs.glsl and basic_fs.glsl: Renderer::instance().allocate_shader_program("basic");
        // To load basic_vs.glsl and nice_fs.glsl: Renderer::instance().allocate_shader_program("basic", "nice");
        //Shader_program allocate_shader_program(const char* vs_name, const char* fs_name = nullptr);

    private :
        Frame_time frame_time_;

        SDL_Window* window_{nullptr};
        SDL_GLContext context_{nullptr};

        //klib::Delegate_list<Render_callback> render_callbacks_;
        //klib::Delegate_list<Update_callback> update_callbacks_;
        std::vector<Render_system*> render_systems_;
    };

	/*template <typename T, void (T::*Fun)()>
	void register_render_callback(T* that)
	{
		Renderer::instance().register_render_callback(Renderer::Render_callback::construct<T, Fun>(that));
	};*/

} // namespace opengl
} // namespace kgfx
