#pragma once
#include "mesh.hpp"

namespace graphics {
namespace bezier {

	template <typename Mesh>
	void triangulate_patch(	Mesh& mesh, 
							const Patch& patch, 
							unsigned num_samples_x, 
							unsigned num_samples_y)
	{
		mesh.vertices.reserve(num_samples_x*num_samples_y);
		mesh.triangles.reserve(num_samples_x*num_samples_y*2);

		const float dx = 1.0f/static_cast<float>(num_samples_x);
		const float dy = 1.0f/static_cast<float>(num_samples_y);

		float ty = 0.0f;
		for (unsigned y = 0; y < num_samples_y + 1; ++y)
		{
			float tx = 0.0f;
			for (unsigned x = 0; x < num_samples_x + 1; ++x)
			{
				tx += dx; 
			}

			ty += dy;
		} 
	}

}
}
