#include <kgfx/opengl/renderer.hpp>
#include <klib/file_io.hpp>
#include "check_opengl_error.hpp"
#include <SDL.h>
#include <memory>
#include <cassert>
#include <algorithm>

namespace kgfx {
namespace opengl {

    void Renderer::run_frame()
    {
        render();
        update();
        present();

        frame_time_.next_frame();
    }

    void Renderer::construct_windowed(unsigned window_width,
                                      unsigned window_height,
                                      const char* window_title) 
    {
        assert(window_ == nullptr);

        window_ = ::SDL_CreateWindow(window_title,
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     window_width,
                                     window_height,
                                     SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

        if (nullptr == window_) {
            throw std::runtime_error("Failed to create window.");
        }

        context_ = ::SDL_GL_CreateContext(window_);
        if (nullptr == context_) {
            throw std::runtime_error("Failed to create OpenGL context.");
        }

        GLenum error = ::glewInit();
        if (error != GLEW_OK) {
            throw std::runtime_error("glewInit failed.");
        }

        ::glEnable(GL_DEPTH_TEST);
        ::glEnable(GL_CULL_FACE);
        ::glFrontFace(GL_CW);
        ::glCullFace(GL_BACK);

        // Enable v-sync.
        ::SDL_GL_SetSwapInterval(1);
    }

    Renderer::~Renderer() 
    {
        if (context_) {
            ::SDL_GL_DeleteContext(context_);
            //context_ = nullptr;
        }

        if (window_) {
            ::SDL_DestroyWindow(window_);
            //window_ = nullptr;
        }
    }

    void clear_buffers()
    {
        glClearColor(0.0, 1.0, 0.0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    /*void Renderer::on_render(Render_callback callback) 
    {
        render_callbacks_.push_back(callback);
    }

    void Renderer::on_update(Update_callback callback) 
    {
        update_callbacks_.push_back(callback);
    }*/

    void Renderer::add_render_system(Render_system* rs)
    {
        render_systems_.push_back(rs);
    }

    void Renderer::remove_render_system(Render_system* rs)
    {
        render_systems_.erase(std::find(render_systems_.begin(), render_systems_.end(), rs));
    }

    void Renderer::render() 
    {
        clear_buffers();

        std::for_each(render_systems_.begin(), 
                      render_systems_.end(), 
                      [] (Render_system* rs) { 
                          rs->render(); 
                      });
    }

    void Renderer::update() 
    {
        std::for_each(render_systems_.begin(), 
                      render_systems_.end(), 
                      [this] (Render_system* rs) { 
                          rs->update(frame_time_); 
                      });
    }

    void Renderer::present() 
    {
        ::SDL_GL_SwapWindow(window_);
    }

    /*Mesh_id Renderer::allocate_mesh(const Triangle_mesh<>& tri_mesh) 
    {
        unsigned mesh_id = invalid_mesh_id;

        if (num_free_ > 0)
        {
            for (unsigned i = 0; i < allocated_meshes_.size(); ++i)
            {
                if (!allocated_meshes_[i].is_valid())
                {
                    --num_free_;
                    mesh_id = i;
                    break;
                }
            }
        }
        else
        {
            allocated_meshes_.emplace_back();
            mesh_id = allocated_meshes_.size() - 1;
        }

        Opengl_mesh& m = allocated_meshes_[mesh_id];

        m.setup_vertex_buffer_object(&tri_mesh.vertices[0], tri_mesh.vertices.size());

        if (!tri_mesh.triangles.empty())
        {
            m.setup_element_buffer_object(&tri_mesh.triangles[0].v0, tri_mesh.triangles.size() * 3);
        }

        m.setup_vertex_array_object();

        return mesh_id;
    }*/

    /*void Renderer::deallocate_mesh(Mesh_id mesh_id)
    {
        allocated_meshes_[mesh_id].destroy();
        ++num_free_;
    }*/

    /*void Renderer::render_mesh(Mesh_id mesh_id) 
    {
        allocated_meshes_[mesh_id].render();
    }*/

//private:
    //using Shader_map = std::map<std::string, std::shared_ptr<Shader_program>>;
    //Shader_map shader_map_;

    /*std::string read_shader_source(const char* pattern, Shader::Shader_type shader_type)
    {
        const std::string resource_path("resources/shaders/");
        const char* suffix = (shader_type == Shader::vertex_shader) ? "_vs.glsl"
                                                                    : "_fs.glsl";
        return klib::read_textfile((resource_path + pattern + suffix).c_str());
    }*/

    /*Shader_program Renderer::allocate_shader_program(const char* vs_name, const char* fs_name) 
    {
        if (nullptr == fs_name)
        {
            fs_name = vs_name;
        }

        const std::string shader_source_vs = read_shader_source(vs_name, Shader::vertex_shader);
        if (shader_source_vs.length() > 0)
        {
            Shader vertex_shader(Shader::vertex_shader,
                                 shader_source_vs.c_str(),
                                 shader_source_vs.length());

            const std::string shader_source_fs = read_shader_source(fs_name, Shader::fragment_shader);
            if (shader_source_fs.length())
            {
                Shader fragment_shader(Shader::fragment_shader,
                                       shader_source_fs.c_str(),
                                       shader_source_fs.length());

                Shader_program program(vertex_shader, fragment_shader);
                if (program)
                {
                    const std::string key(std::string(vs_name) + fs_name);
                    std::shared_ptr<Shader_program>& program_ref = shader_map_[key];
                    program_ref = std::make_shared<Shader_program>();
                    program_ref->swap(program);
                    return program_ref;
                }
            }
        }

        return std::shared_ptr<Shader_program>();
    } */

    /*Renderer& Renderer::instance()
    {
        static Opengl_renderer renderer;
        return renderer;
    }*/

} // namespace opengl
} // namespace kgfx
