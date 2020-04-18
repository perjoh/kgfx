#pragma once
#include "my_glm.hpp"
#include <vector>

namespace kgfx {

    //
    inline glm::vec3 calculate_normal(const glm::vec3& v0,
                                      const glm::vec3& v1,
                                      const glm::vec3& v2)
    {
        return glm::normalize(glm::cross(v1 - v0, v2 - v0));
    }

    struct Vertex {

        Vertex() = default;

        Vertex(const glm::vec3& pos)
            : position(pos)
            , normal(0.0)
            , color(0.0)
        {
        }

        Vertex(const glm::vec3& pos,
               const glm::vec3& norm,
               const glm::vec3& col)
            : position(pos)
            , normal(norm)
            , color(col)
        {
        }

        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
    };

    struct Triangle {
        Triangle() = default;

        Triangle(unsigned v0_,
                 unsigned v1_,
                 unsigned v2_)
            : v0(v0_)
            , v1(v1_)
            , v2(v2_)
        {
        }

        Triangle offset(unsigned i) const
        {
            Triangle t(*this);
            t.v0 += i;
            t.v1 += i;
            t.v2 += i;
            return t;
        }

        // Replace 'a' with 'b'.
        void replace(unsigned a, unsigned b)
        {
            if (v0 == a)
                v0 = b;

            if (v1 == a)
                v1 = b;

            if (v2 == a)
                v2 = b;
        }

        unsigned v0{~0u};
        unsigned v1{~0u};
        unsigned v2{~0u};
    };

    typedef std::vector<Triangle> Triangle_array;

    //
    template <typename Vertex = kgfx::Vertex>
    struct Triangle_mesh {

        Triangle_mesh() = default;

        Triangle_mesh(Triangle_mesh&& other) noexcept
            : vertices(std::move(other.vertices))
            , triangles(std::move(other.triangles))
        {
        }

        static Triangle_mesh import_raw(const float* vertices_ptr, 
                                        std::size_t vertices_size, 
                                        const int* indices_ptr, 
                                        std::size_t indices_size)
        {
            Triangle_mesh tmp;
            tmp.vertices.reserve(vertices_size/3);
            for (std::size_t i = 0; i < vertices_size; i += 3) {
                tmp.vertices.push_back(glm::vec3(vertices_ptr[i+0], 
                                                 vertices_ptr[i+1], 
                                                 vertices_ptr[i+2]));
            }

            tmp.triangles.reserve(indices_size/3);
            for (std::size_t i = 0; i < indices_size; i += 3) {
                tmp.triangles.push_back(Triangle(indices_ptr[i+0],
                                                 indices_ptr[i+1],
                                                 indices_ptr[i+2]));
            }

            tmp.calculate_vertex_normals();

            return tmp;
        }

        std::vector<Vertex> vertices;
        Triangle_array triangles;

        //
        template <typename Fun>
        void foreach_vertex(Fun f)
        {
            for (auto& v : vertices)
            {
                f(v);
            }
        }

        //
        void transform(const glm::mat4& m)
        {
            foreach_vertex([&m](Vertex& v) {
                glm::vec4 pos(v.position, 1.0f);
                pos = m * pos;
                v.position = glm::vec3(pos);
            });
        }

        //
        void scale(const glm::vec3& factor)
        {
            foreach_vertex([&factor](Vertex& v) { v.position *= factor; });
        }

        //
        void translate(const glm::vec3& delta)
        {
            foreach_vertex([&delta](Vertex& v) { v.position += delta; });
        }

        //
        void set_color(const glm::vec3& color)
        {
            for (auto& vertex : vertices) {
                vertex.color = color;
            }
        }

        //
        void calculate_vertex_normals()
        {
            if (!triangles.empty())
            {
                // Initialize
                for (auto& v : vertices)
                {
                    v.normal = glm::vec3(0.0f);
                }

                std::vector<float> denom(vertices.size(), 0.0f);

                // Count and sum.
                for (const Triangle& t : triangles)
                {
                    auto p0{vertices[t.v0].position};
                    auto p1{vertices[t.v1].position};
                    auto p2{vertices[t.v2].position};

                    auto normal = calculate_normal(p0, p1, p2);

                    vertices[t.v0].normal += normal;
                    vertices[t.v1].normal += normal;
                    vertices[t.v2].normal += normal;

                    denom[t.v0] += 1.0f;
                    denom[t.v1] += 1.0f;
                    denom[t.v2] += 1.0f;
                }

                // Divide
                for (unsigned i = 0; i < denom.size(); ++i)
                {
                    vertices[i].normal /= denom[i];
                    glm::normalize(vertices[i].normal);
                }
            }
            else
            {
                assert(vertices.size() % 3 == 0);

                for (size_t i = 0; i < vertices.size(); i += 3)
                {
                    const auto normal = calculate_normal(vertices[i + 0].position,
                                                         vertices[i + 1].position,
                                                         vertices[i + 2].position);

                    vertices[i + 0].normal = normal;
                    vertices[i + 1].normal = normal;
                    vertices[i + 2].normal = normal;
                }
            }
        }

        //
        void merge(const Triangle_mesh& other)
        {
            size_t prev_num_vertices = vertices.size();
            std::copy(other.vertices.begin(), other.vertices.end(), std::back_inserter(vertices));

            for (const auto& triangle : other.triangles)
            {
                triangles.push_back(triangle.offset(prev_num_vertices));
            }
        }

        //
        void merge_triangles(const std::vector<glm::vec3>& vertices_src)
        {
            assert(vertices_src.size() % 3 == 0);

            vertices.reserve(vertices.size() + vertices_src.size());

            for (const auto& vert_src : vertices_src)
            {
                Vertex vert_new;
                vert_new.position = vert_src;
                vertices.push_back(vert_new);
            }
        }

        // Removes all "equal" vertices.
        void optimize()
        {
            for (size_t a = 0; a < vertices.size() - 1; ++a)
            {
                for (size_t b = a + 1; b < vertices.size(); ++b)
                {
                    if (same_position(vertices[a].position, vertices[b].position))
                    {
                        replace_vertex(a, b);
                        --b; // Since one vertex is removed.
                    }
                }
            }
        }

        //
        void make_non_indexed()
        {
            std::vector<Vertex> tmp_vertices;
            tmp_vertices.reserve(vertices.size());

            for (const auto& triangle : triangles)
            {
                tmp_vertices.push_back(vertices[triangle.v0]);
                tmp_vertices.push_back(vertices[triangle.v1]);
                tmp_vertices.push_back(vertices[triangle.v2]);
            }

            vertices.swap(tmp_vertices);

            Triangle_array tmp;
            triangles.swap(tmp);
        }

    public:
        //
        template <typename Patch>
        void make_patch(const Patch& patch,
                        unsigned num_samples_x,
                        unsigned num_samples_y)
        {
            // Triangles.
            const size_t triangle_offset = vertices.size();
            triangles.reserve(triangle_offset + num_samples_x * num_samples_y * 2);
            for (unsigned y = 0; y < num_samples_y - 1; ++y)
            {
                for (unsigned x = 0; x < num_samples_x - 1; ++x)
                {
                    const unsigned v0 = triangle_offset + x + y * num_samples_x;
                    triangles.push_back({v0, v0 + 1, v0 + 1 + num_samples_x});
                    triangles.push_back({v0 + 1 + num_samples_x, v0 + num_samples_x, v0});
                }
            }

            // Vertices.
            vertices.reserve(vertices.size() + num_samples_x * num_samples_y);

            const float dx = 1.0f / static_cast<float>(num_samples_x - 1);
            const float dy = 1.0f / static_cast<float>(num_samples_y - 1);

            float ty = 0.0f;
            for (unsigned y = 0; y < num_samples_y; ++y)
            {
                float tx = 0.0f;
                for (unsigned x = 0; x < num_samples_x; ++x)
                {
                    Vertex v;
                    v.position = patch.sample(tx, ty);
                    vertices.push_back(v);

                    tx += dx;
                }

                ty += dy;
            }
        }

    private:
        //
        static bool same_position(const Vertex& a, const Vertex& b)
        {
            using Float = decltype(Vertex::position.x);
            static const Float threshold = Float(1.0 / 10000 * 2);
            const auto d = b.position - a.position;
            return (d.x * d.x + d.y * d.y + d.z * d.z) < threshold;
        }

        //
        void replace_vertex(size_t keep, size_t replace)
        {
            assert(keep < replace);

            vertices[replace] = vertices.back();
            vertices.pop_back();

            for (auto& triangle : triangles)
            {
                triangle.replace(replace, keep);
                triangle.replace(vertices.size(), replace);
            }
        }
    };

} // namespace kgfx
