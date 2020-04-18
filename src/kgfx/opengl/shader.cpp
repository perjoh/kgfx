#include "check_opengl_error.hpp"
#include <kgfx/opengl/shader.hpp>
#include <klib/file_io.hpp>
#include <cassert>
#include <vector>
#include <stdexcept>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace kgfx {
namespace opengl { 

    GLuint create_shader_from_source(GLenum shader_type,
                                     const char* source,
                                     GLint shader_source_len = 0)
    {
        if (nullptr == source) {
            return 0;
        }

        // Compile source code
        GLuint shader = glCreateShader(shader_type);
        const GLint* length = (shader_source_len == 0) ? nullptr : &shader_source_len;
        glShaderSource(shader, 1, &source, length);
        glCompileShader(shader);

        check_opengl_error();

        // Get status of compilation
        GLint status = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (GL_FALSE == status)
        {
            GLint infoLogLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
            std::vector<GLchar> infoLog(infoLogLength + 1);
            glGetShaderInfoLog(shader, infoLogLength, nullptr, &infoLog[0]);

            throw std::runtime_error(&infoLog[0]);
        }

        return shader;
    }

    GLenum opengl_shader_type(Shader::Shader_type shader_type)
    {
        return (shader_type == Shader::vertex_shader) ? GL_VERTEX_SHADER
                                                      : GL_FRAGMENT_SHADER;
    }

    Shader::Shader(Shader_type shader_type,
                   const char* shader_source,
                   int shader_source_len)
        : handle_{create_shader_from_source(opengl_shader_type(shader_type),
                                            shader_source,
                                            shader_source_len)}
    {
    }

    Shader::Shader(Shader&& rhs) noexcept
        : handle_(rhs.handle_)
    {
        rhs.handle_ = 0;
    }

    Shader::~Shader()
    {
        destroy();
    }

    Shader& Shader::operator=(Shader&& rhs)
    {
        if (this != &rhs) {
            destroy();
            handle_ = rhs.handle_;
            rhs.handle_ = 0;
        }
        return *this;
    }

    Shader Shader::load_from_source_file(const std::string& filename)
    { 
        Shader_type shader_type = invalid_shader;
        if (filename.find(".vs.") != std::string::npos) {
            shader_type = vertex_shader;
        } else if (filename.find(".fs.") != std::string::npos) {
            shader_type = fragment_shader;
        } else {
            throw std::runtime_error("Invalid shader filename. Cannot determine shader type.");
        }

        auto source = klib::read_textfile(filename.c_str());
        Shader shader(shader_type, source.c_str());
        return shader;
    }

    Shader::operator bool() const
    {
        return handle_ != 0;
    }

    void Shader::destroy()
    {
        if (handle_ != 0)
        {
            ::glDeleteShader(handle_);
            handle_ = 0;
        }
    }

    GLuint link_shader_program(GLuint vertex_shader_handle,
                               GLuint fragment_shader_handle)
    {
        GLuint program_handle = ::glCreateProgram();

        ::glAttachShader(program_handle, vertex_shader_handle);
        ::glAttachShader(program_handle, fragment_shader_handle);

        ::glLinkProgram(program_handle);

        check_opengl_error();

        // Get linking status
        GLint status = GL_FALSE;
        ::glGetProgramiv(program_handle,
                         GL_LINK_STATUS,
                         &status);

        if (status == GL_FALSE)
        {
            GLint log_length = 0;
            ::glGetProgramiv(program_handle,
                             GL_INFO_LOG_LENGTH,
                             &log_length);

            std::vector<char> infoLog(log_length);

            ::glGetProgramInfoLog(program_handle,
                                  log_length,
                                  nullptr,
                                  &infoLog[0]);

            throw std::runtime_error(&infoLog[0]);
        }

        ::glDetachShader(program_handle, fragment_shader_handle);
        ::glDetachShader(program_handle, vertex_shader_handle);

        return program_handle;
    }

    Shader_scope::Shader_scope(Shader_program* shader)
        : shader_(shader)
    { 
        shader_->bind();
    }

    Shader_scope::Shader_scope(Shader_scope&& rhs)
        : shader_(std::move(rhs.shader_))
    { 
    }

    Shader_scope::~Shader_scope()
    {
        if (shader_) {
            shader_->unbind();
        }
    }

    Shader_program::Shader_program(const Shader& vertex_shader,
                                   const Shader& fragment_shader)
        : handle_{link_shader_program(vertex_shader.handle_,
                                      fragment_shader.handle_)}
    {
    }

    Shader_program::Shader_program(Shader_program&& rhs)
        : handle_(rhs.handle_) 
    {
        rhs.handle_ = 0;
    }

    Shader_program::~Shader_program()
    {
        destroy();
    }

    Shader_program& Shader_program::operator=(Shader_program&& rhs)
    {
        if (this != &rhs) {
            destroy();
            handle_ = rhs.handle_;
            rhs.handle_ = 0;
        }
        return *this;
    }


    void Shader_program::swap(Shader_program& rhs)
    {
        std::swap(handle_, rhs.handle_);
    }


    Shader_scope Shader_program::bind_scope()
    {
        return Shader_scope(this);
    }

    Shader_program::operator bool() const
    {
        return handle_ != 0;
    }

    void Shader_program::bind()
    {
        assert(handle_ != 0);
        ::glUseProgram(handle_);
        check_opengl_error();
    }

    void Shader_program::unbind()
    {
        ::glUseProgram(0);
    }
    
    void Shader_program::destroy()
    {
        if (handle_ != 0)
        {
            ::glDeleteProgram(handle_);
            handle_ = 0;
        }
    }

    int Shader_program::get_uniform_location(const char* name) const
    {
        return ::glGetUniformLocation(handle_, name);
    }

    template <typename T>
    Shader_uniform<T>::Shader_uniform(int location)
        : location_(location)
    {
        assert(location != -1 && "Invalid location");
    }

    void set_uniform(GLuint location,
                     const float& value)
    {
        ::glUniform1f(location, value);
    }

    void set_uniform(GLuint location,
                     const glm::vec3& value)
    {
        ::glUniform3f(location,
                      value[0],
                      value[1],
                      value[2]);
    }

    void set_uniform(GLuint location,
                     const glm::vec4& value)
    {
        // glUniform4fv better?
        ::glUniform4f(location,
                      value[0],
                      value[1],
                      value[2],
                      value[3]);
    }

    void set_uniform(GLuint location,
                     const glm::mat3& value)
    {
        ::glUniformMatrix3fv(location,
                             1,
                             GL_FALSE,
                             glm::value_ptr(value));
    }

    void set_uniform(GLuint location,
                     const glm::mat4& value)
    {
        ::glUniformMatrix4fv(location,
                             1,
                             GL_FALSE,
                             glm::value_ptr(value));
    }

    template <typename T>
    void Shader_uniform<T>::set(const T& value)
    {
        set_uniform(location_, value);
    }

    template <typename T>
    bool Shader_uniform<T>::is_valid() const
    {
        return location_ != -1;
    }

    template Shader_uniform<float>;
    template Shader_uniform<glm::vec3>;
    template Shader_uniform<glm::vec4>;
    template Shader_uniform<glm::mat3>;
    template Shader_uniform<glm::mat4>;

} // namespace opengl
} // namespace kgfx
