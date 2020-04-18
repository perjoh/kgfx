#pragma once
#include <GL/glew.h>
#include <stdexcept>
#include <string>

namespace kgfx {

    inline void check_opengl_error()
    {
        const GLenum error = ::glGetError();
        if (error != GL_NO_ERROR)
        {
            const char* str = reinterpret_cast<const char*>(::gluErrorString(error));

            throw std::runtime_error(std::string("OpenGL error: ") + str);
        }
    }

} // namespace kgfx
