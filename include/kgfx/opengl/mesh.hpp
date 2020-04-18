#pragma once
#include <GL/glew.h>
#include "../mesh.hpp"

namespace kgfx {
namespace opengl {
    
    class Render;

    class Mesh 
    {
    public:
        Mesh() = default;
        Mesh(const Mesh&) = delete;
        Mesh(const Triangle_mesh<>&);
        Mesh(Mesh&& other) noexcept;

        ~Mesh();

        Mesh& operator=(const Mesh&) = delete;

        explicit operator bool() const;

        void swap(Mesh&);

    public :
        void load_mesh(const Triangle_mesh<>&);

    public: 
        void render();

    private :
        //void set_draw_mode(GLenum draw_mode);

        //typedef graphics::mesh::Vertex Vertex;
        void setup_vertex_buffer_object(const Vertex* vertices, size_t vertex_count);
        void setup_element_buffer_object(const GLuint* indices, size_t index_count);
        void setup_vertex_array_object(); 

    private: 
        void destroy();

        GLuint vertex_buffer_object_{0};
        GLuint vertex_array_object_{0};
        GLuint element_buffer_object_{0};

        GLuint render_count_{0};

        GLenum draw_mode_{GL_TRIANGLES};
    };

} // namespace opengl
} // namespace kgfx
