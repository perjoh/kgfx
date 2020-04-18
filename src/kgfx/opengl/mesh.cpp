#include <kgfx/opengl/mesh.hpp>
#include "check_opengl_error.hpp"

namespace kgfx {
namespace opengl {

    Mesh::Mesh(const Triangle_mesh<>& source)
    { 
        setup_vertex_buffer_object(&source.vertices[0], 
                                   source.vertices.size());

        if (!source.triangles.empty()) {
            setup_element_buffer_object(&source.triangles[0].v0, 
                                        source.triangles.size()*3);
        }

        setup_vertex_array_object();
    }

    Mesh::Mesh(Mesh&& rhs) noexcept
        : vertex_buffer_object_{rhs.vertex_buffer_object_}
        , vertex_array_object_{rhs.vertex_array_object_}
        , element_buffer_object_{rhs.element_buffer_object_}
        , render_count_{rhs.render_count_}
    {
        rhs.vertex_buffer_object_ = 0;
        rhs.vertex_array_object_ = 0;
        rhs.element_buffer_object_ = 0;
        rhs.render_count_ = 0;
    }

    Mesh::~Mesh()
    {
        destroy();
    }

    void Mesh::destroy()
    {
        if (element_buffer_object_ != 0)
        {
            ::glDeleteBuffers(1, &element_buffer_object_);
            element_buffer_object_ = 0;
        }

        if (vertex_buffer_object_ != 0)
        {
            ::glDeleteBuffers(1, &vertex_buffer_object_);
            vertex_buffer_object_ = 0;
        }

        if (vertex_array_object_ != 0)
        {
            ::glDeleteVertexArrays(1, &vertex_array_object_);
            vertex_array_object_ = 0;
        }
    }

    Mesh::operator bool() const
    {
        return vertex_array_object_ != 0;
    }

    void Mesh::swap(Mesh& rhs)
    {
        std::swap(vertex_buffer_object_, rhs.vertex_buffer_object_);
        std::swap(vertex_array_object_, rhs.vertex_array_object_);
        std::swap(element_buffer_object_, rhs.element_buffer_object_);

        std::swap(render_count_, rhs.render_count_);

        std::swap(draw_mode_, rhs.draw_mode_);
    }

    void Mesh::load_mesh(const Triangle_mesh<>& source)
    {
        destroy();

        Mesh mesh(source);
        if (mesh) {
            mesh.swap(*this);
        }
    }

    /*void Mesh::set_draw_mode(GLenum draw_mode)
    {
        draw_mode_ = draw_mode;
    }*/

    void Mesh::setup_vertex_buffer_object(const Vertex* vertices,
                                          size_t vertex_count)
    {
        ::glGenBuffers(1, &vertex_buffer_object_);

        ::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);

        const GLsizeiptr dataSize = sizeof(Vertex) * vertex_count;

        ::glBufferData(GL_ARRAY_BUFFER,
                       dataSize,
                       &vertices[0],
                       GL_STATIC_DRAW);

        ::glBindBuffer(GL_ARRAY_BUFFER, 0);

        check_opengl_error();

        render_count_ = static_cast<GLuint>(vertex_count);
    }

    void Mesh::setup_element_buffer_object(const GLuint* indices,
                                                  size_t index_count)
    {
        assert(indices);

        ::glGenBuffers(1, &element_buffer_object_);
        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);

        const size_t index_buffer_size = sizeof(GLuint) * index_count;
        ::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                       index_buffer_size,
                       &indices[0],
                       GL_STATIC_DRAW);

        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        render_count_ = static_cast<GLuint>(index_count); // ???

        check_opengl_error();
    }

    void Mesh::setup_vertex_array_object()
    {
        assert(vertex_array_object_ == 0);
        assert(vertex_buffer_object_ != 0);

        ::glGenVertexArrays(1, &vertex_array_object_);
        ::glBindVertexArray(vertex_array_object_);

        check_opengl_error();

        // Vertex Buffer Object
        ::glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);

        check_opengl_error();

        // Element Buffer Object
        if (element_buffer_object_ != 0)
        {
            ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object_);
        }

        const unsigned NUM_ARRAYS = 3;
        for (GLuint index = 0, offset = 0;
             index < NUM_ARRAYS;
             ++index)
        {
            ::glEnableVertexAttribArray(index);

            const GLboolean normalize = GL_FALSE;
            ::glVertexAttribPointer(index,
                                    3,
                                    GL_FLOAT,
                                    normalize,
                                    sizeof(Vertex),
                                    reinterpret_cast<const GLvoid*>(static_cast<unsigned long long>(offset)));

            offset += sizeof(glm::vec3);

            check_opengl_error();
        }

        ::glBindVertexArray(0);
    }

    void Mesh::render()
    {
        ::glBindVertexArray(vertex_array_object_);

        if (0 == element_buffer_object_)
        {
            // Individual triangles
            ::glDrawArrays(draw_mode_, 0, render_count_);
        }
        else
        {
            // Indexed triangles
            ::glDrawElements(GL_TRIANGLES, render_count_, GL_UNSIGNED_INT, 0);
        }

        check_opengl_error();

        ::glBindVertexArray(0);
    }

} // namespace opengl
} // namespace kgfx
