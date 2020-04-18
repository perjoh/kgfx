#pragma once
#include <array>
#include <initializer_list>
#include <vector>

namespace kgfx {

    namespace detail {

        template <typename Point, typename T>
        inline Point lerp(const Point& a, const Point& b, T t)
        {
            return a * (T(1) - t) + b * t;
        }

        template <typename Point, typename T>
        inline Point lerp(const Point& a, const Point& b, T t0, T t1)
        {
            // assert: t0 + t1 == 1
            return a * t0 + b * t1;
        }

        template <typename T>
        T pow2(T t)
        {
            return t * t;
        }

        template <typename T>
        T pow3(T t)
        {
            return pow2(t) * t;
        }

        template <typename T>
        T pow4(T t)
        {
            return pow2(t) * pow2(t);
        }

        template <int degree, int n>
        struct Bernstein {
            template <typename T>
            static T value(T x);
        };

        template <int n>
        struct Bernstein<0, n> {
            template <typename T>
            static T value(T x)
            {
                return T(1);
            }
        };

        template <int n>
        struct Bernstein<1, n> {
            template <typename T>
            static T value(T x)
            {
                switch (n) {
                case 0:
                    return T(1) - x;
                case 1:
                    return x;
                }

                return T(0);
            }
        };

        template <int n>
        struct Bernstein<2, n> {
            template <typename T>
            static T value(T x)
            {
                switch (n) {
                case 0:
                    return pow2(T(1) - x);
                case 1:
                    return T(2) * x * (T(1) - x);
                case 2:
                    return pow2(x);
                }

                return T(0);
            }
        };

        template <int n>
        struct Bernstein<3, n> {
            template <typename T>
            static T value(T x)
            {
                switch (n) {
                case 0:
                    return pow3(T(1) - x);
                case 1:
                    return T(3) * x * pow2(T(1) - x);
                case 2:
                    return T(3) * pow2(x) * (T(1) - x);
                case 3:
                    return pow3(x);
                }

                return T(0);
            }
        };

        template <int n>
        struct Bernstein<4, n> {
            template <typename T>
            static T value(T x)
            {
                switch (n) {
                case 0:
                    return pow4(1.0 - x);
                case 1:
                    return T(4) * x * pow3(T(1) - x);
                case 2:
                    return T(6) * pow2(x) * pow2(T(1) - x);
                case 3:
                    return T(4) * pow3(x) * (T(1) - x);
                case 4:
                    return pow4(x);
                }

                return T(0);
            }
        };

        template <typename Point, typename T, int degree, int n>
        struct Curve_sampler {

            static Point sum(const Point* points, T t)
            {
                return points[0] * Bernstein<degree, degree - n>::value(t) + Curve_sampler<Point, T, degree, n - 1>::sum(points + 1, t);
            }

            static Point sum_weights(const Point* points, const T* weights, T t)
            {
                return points[0] * Bernstein<degree, degree - n>::value(t) * weights[0] + Curve_sampler<Point, T, degree, n - 1>::sum_weights(points + 1, weights + 1, t);
            }
        };

        template <typename Point, typename T, int degree>
        struct Curve_sampler<Point, T, degree, 0> {
            static Point sum(const Point* points, T t)
            {
                return points[0] * Bernstein<degree, degree>::value(t);
            }

            static Point sum_weights(const Point* points, const T* weights, T t)
            {
                return points[0] * Bernstein<degree, degree>::value(t) * weights[0];
            }
        };

        template <typename Point, typename T, int j = 3, int i = 3>
        struct Surface_sampler {
            static Point sum(const Point* points, T u, T v)
            {
                return points[i + j * 4] * Bernstein<3, i>::value(u) * Bernstein<3, j>::value(v)
                    + Surface_sampler<Point, T, j, i - 1>::sum(points, u, v);
            }

            static Point fun(const Point* points, T u, T v)
            {
                return points[i + j * 4] * Bernstein<3, i>::value(u) * Bernstein<3, j>::value(v);
            }
        };

        template <typename Point, typename T, int j>
        struct Surface_sampler<Point, T, j, 0> {
            static Point sum(const Point* points, T u, T v)
            {
                return points[0 + j * 4] * Bernstein<3, 0>::value(u) * Bernstein<3, j>::value(v)
                    + Surface_sampler<Point, T, j - 1, 3>::sum(points, u, v);
            }
        };

        template <typename Point, typename T>
        struct Surface_sampler<Point, T, 0, 0> {
            static Point sum(const Point* points, T u, T v)
            {
                return points[0] * Bernstein<3, 0>::value(u) * Bernstein<3, 0>::value(v);
            }
        };

        template <int degree, typename Point, typename T>
        inline Point sample(const Point* points, T t)
        {
            return Curve_sampler<Point, T, degree, degree>::sum(&points[0], t);
        }

        template <int degree, typename Point, typename T>
        inline Point sample_weight(const Point* points, const T* weights, T t)
        {
            return Curve_sampler<Point, T, degree, degree>::sum_weights(&points[0], &weights[0], t) / sample<degree, T, T>(&weights[0], t);
        }

        template <typename Point, typename T>
        inline Point sample_patch(const Point* patch, T t0, T t1)
        {
            return Surface_sampler<Point, T>::sum(patch, t0, t1);
            /*const Point tmp[4] = { 	sample<3>(&patch[0], t0), 
									sample<3>(&patch[4], t0), 
									sample<3>(&patch[8], t0), 
									sample<3>(&patch[12], t0) };

			return sample<3>(&tmp[0], t1);*/
        }

    } // namespace detail

    template <typename Point, typename T, size_t size>
    Point sample(const std::array<Point, size>& points, T t)
    {
        return detail::sample(&points[0], t);
    }

    template <typename Point, typename T, size_t size>
    Point sample_weight(const std::array<Point, size>& points, const std::array<T, size>& weights, T t)
    {
        return detail::sample_weight<size - 1>(&points[0], &weights[0], t);
    }

    using detail::sample_patch;

    /*template <typename Point, typename T>
	inline Point sample_patch_weight(const Point* patch, const T* weights, T t0, T t1)
	{
		const Point tmp[4] = { 	sample_weight<3>(&patch[0], 	&weights[0], 	t0), 
								sample_weight<3>(&patch[4], 	&weights[4], 	t0), 
								sample_weight<3>(&patch[8], 	&weights[8], 	t0), 
								sample_weight<3>(&patch[12], 	&weights[12], 	t0) };

		return sample<3>(&tmp[0], t1);
	}*/

    //
    template <typename Point, typename T, int degree>
    class Bezier_curve {
    public:
        Bezier_curve(const std::array<Point, degree>& points, 
					 const std::array<T, degree>& weights)
            : points_(points)
            , weights_(weights)
        {
        }

    public:
        inline Point sample(T t) const
        {
            return sample_weight(points_, weights_, t);
        }

        const auto& get_points() const
        {
            return points_;
        }

    private:
        std::array<Point, degree> points_;
        std::array<T, degree> weights_;
    };

    //
    template <typename Point, typename T>
    class Bezier_patch {
    public:
        Bezier_patch(const std::array<Point, 4 * 4>& points)
            : points_(points)
        {
        }

    public:
        Point sample(T tx, T ty) const
        {
            return sample_patch(&points_[0], tx, ty);
        }

        const auto& get_points() const
        {
            return points_;
        }

    private:
        std::array<Point, 4 * 4> points_;
    };

} // namespace kgfx
