#pragma once
#include <string>

namespace kgfx {
namespace opengl {

    //
    class Shader {
        friend class Shader_program;

    public:
        enum Shader_type { vertex_shader,
                           fragment_shader,
                           invalid_shader };

        Shader(Shader_type shader_type,
               const char* shader_source,
               int shader_source_len = 0);

        Shader(Shader&&) noexcept;
        Shader(const Shader&) = delete;
        ~Shader();

        Shader& operator=(const Shader&) = delete;
        Shader& operator=(Shader&&);

        explicit operator bool() const;

    public :
        // Note: Determines shader type by checking if ".vs." or ".fs." is present in the filename.
        static Shader load_from_source_file(const std::string& filename);

    private:
        void destroy();

        unsigned int handle_{0};
    };

    //
    class Shader_program;
    class Shader_scope
    {
    private :
        friend class Shader_program;

        Shader_scope(Shader_program* shader);

    public :
        Shader_scope(Shader_scope&& rhs); 
        ~Shader_scope();

    private :
        Shader_program* shader_{nullptr};
    };

    template <typename T>
    class Shader_uniform;

    //
    class Shader_program 
    {
        friend class Shader_scope;

    public:
        Shader_program() = default;
        Shader_program(const Shader_program&) = delete;
        Shader_program(Shader_program&&);

        Shader_program(const Shader& vertex_shader,
                       const Shader& fragment_shader);

        ~Shader_program();

    public:
        Shader_program& operator=(const Shader_program&) = delete;
        Shader_program& operator=(Shader_program&&);

		explicit operator bool() const;

        void swap(Shader_program&);

    public:
        template <typename T>
        Shader_uniform<T> get_uniform(const char* uniform_name) const
        {
            return Shader_uniform<T>(get_uniform_location(uniform_name));
        } 

    public :
        Shader_scope bind_scope();

    private:
        void bind();
        void unbind();

        void destroy();

    private:
		int get_uniform_location(const char* name) const;	

        unsigned int handle_{0};
    };

    template <typename T>
    class Shader_uniform {
    private:
        friend class Shader_program;

        Shader_uniform(int location);

    public:
		Shader_uniform() = default;

        void set(const T&);

		bool is_valid() const;

    private:
		int location_{ -1 };
    };

} // namespace opengl
} // namespace kgfx
