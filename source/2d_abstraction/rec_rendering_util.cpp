/**
*************************************************************************
*
* @file rec_rendering_util.cpp
*
* 
*
************************************************************************/

#include "rec_rendering_util.h"


namespace rec {





	void addLine(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh,
		viral_core::vector vec_one, viral_core::vector vec_two) {

		int start_index = geometry_mesh->vertex_count();

		viral_core::mesh::const_stream_iterator normals_it = geometry_mesh->find_vertex_stream(viral_core::mesh_stream_registry::normal_stream_name);
		if (!normals_it) {
			viral_core::auto_pointer<viral_core::mesh_stream_vector> new_normals(new viral_core::mesh_stream_vector());
			normals_it = geometry_mesh->insert_vertex_stream(viral_core::mesh_stream_registry::normal_stream_name, new_normals);
		}
		viral_core::mesh_stream_vector& normals = static_cast<viral_core::mesh_stream_vector&>(*normals_it->value);



		geometry_mesh->vertices().push_back(vec_one);	//0
		geometry_mesh->vertices().push_back(vec_two);	//1

		geometry_mesh->lines().push_back(viral_core::mesh_line(start_index + 0, start_index + 1));

		normals.push_back(viral_core::vector(0, 0, -1).normalized());
		normals.push_back(viral_core::vector(0, 0, -1).normalized());
	}







	void addSquare(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh,
		viral_core::vector top_left, viral_core::vector top_right, viral_core::vector bot_left, viral_core::vector bot_right) {

		int start_index = geometry_mesh->vertex_count();

		viral_core::mesh::const_stream_iterator normals_it = geometry_mesh->find_vertex_stream(viral_core::mesh_stream_registry::normal_stream_name);
		if (!normals_it) {
			viral_core::auto_pointer<viral_core::mesh_stream_vector> new_normals(new viral_core::mesh_stream_vector());
			normals_it = geometry_mesh->insert_vertex_stream(viral_core::mesh_stream_registry::normal_stream_name, new_normals);
		}
		viral_core::mesh_stream_vector& normals = static_cast<viral_core::mesh_stream_vector&>(*normals_it->value);



		geometry_mesh->vertices().push_back(top_left);	//0, top_left
		geometry_mesh->vertices().push_back(top_right);	//1, top_right
		geometry_mesh->vertices().push_back(bot_left);	//2, bot_left
		geometry_mesh->vertices().push_back(bot_right);	//3, bot_right

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 0, start_index + 2, start_index + 1));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 1, start_index + 2, start_index + 3));

		normals.push_back(viral_core::vector(0, 0, -1).normalized());
		normals.push_back(viral_core::vector(0, 0, -1).normalized());
		normals.push_back(viral_core::vector(0, 0, -1).normalized());
		normals.push_back(viral_core::vector(0, 0, -1).normalized());

	}







	void addCubeAroundVector(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh, viral_core::vector vec, float size) {

		int start_index = geometry_mesh->vertex_count();

		viral_core::mesh::const_stream_iterator normals_it = geometry_mesh->find_vertex_stream(viral_core::mesh_stream_registry::normal_stream_name);
		if (!normals_it) {
			viral_core::auto_pointer<viral_core::mesh_stream_vector> new_normals(new viral_core::mesh_stream_vector());
			normals_it = geometry_mesh->insert_vertex_stream(viral_core::mesh_stream_registry::normal_stream_name, new_normals);
		}
		viral_core::mesh_stream_vector& normals = static_cast<viral_core::mesh_stream_vector&>(*normals_it->value);



		// === front face ===
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y - size, vec.z - size));	//0, near_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y - size, vec.z - size));	//1, near_top_right
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y + size, vec.z - size));	//2, near_bot_left
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y + size, vec.z - size));	//3, near_bot_right

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 0, start_index + 2, start_index + 1));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 1, start_index + 2, start_index + 3));

		normals.push_back(viral_core::vector(0, 0, -1).normalized());
		normals.push_back(viral_core::vector(0, 0, -1).normalized());
		normals.push_back(viral_core::vector(0, 0, -1).normalized());
		normals.push_back(viral_core::vector(0, 0, -1).normalized());


		// === right face ===
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y - size, vec.z - size));	//4, near_top_right
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y - size, vec.z + size));	//5, far_top_right
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y + size, vec.z - size));	//6, near_bot_right
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y + size, vec.z + size));	//7, far_bot_right

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 4, start_index + 6, start_index + 5));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 5, start_index + 6, start_index + 7));

		normals.push_back(viral_core::vector(1, 0, 0).normalized());
		normals.push_back(viral_core::vector(1, 0, 0).normalized());
		normals.push_back(viral_core::vector(1, 0, 0).normalized());
		normals.push_back(viral_core::vector(1, 0, 0).normalized());


		// === back face === 
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y - size, vec.z + size));	//8, far_top_right
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y - size, vec.z + size));	//9, far_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y + size, vec.z + size));	//10, far_bot_right
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y + size, vec.z + size));	//11, far_bot_left

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 8, start_index + 10, start_index + 9));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 9, start_index + 10, start_index + 11));

		normals.push_back(viral_core::vector(0, 0, 1).normalized());
		normals.push_back(viral_core::vector(0, 0, 1).normalized());
		normals.push_back(viral_core::vector(0, 0, 1).normalized());
		normals.push_back(viral_core::vector(0, 0, 1).normalized());


		// === left face === 
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y - size, vec.z + size));	//12, far_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y - size, vec.z - size));	//13, near_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y + size, vec.z + size));	//14, far_bot_left
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y + size, vec.z - size));	//15, near_bot_left

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 12, start_index + 14, start_index + 13));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 13, start_index + 14, start_index + 15));

		normals.push_back(viral_core::vector(-1, 0, 0).normalized());
		normals.push_back(viral_core::vector(-1, 0, 0).normalized());
		normals.push_back(viral_core::vector(-1, 0, 0).normalized());
		normals.push_back(viral_core::vector(-1, 0, 0).normalized());


		// === top face === 
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y - size, vec.z + size));	//16, far_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y - size, vec.z + size));	//17, far_top_right
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y - size, vec.z - size));	//18, near_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y - size, vec.z - size));	//19, near_top_right

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 16, start_index + 18, start_index + 17));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 17, start_index + 18, start_index + 19));

		normals.push_back(viral_core::vector(0, -1, 0).normalized());
		normals.push_back(viral_core::vector(0, -1, 0).normalized());
		normals.push_back(viral_core::vector(0, -1, 0).normalized());
		normals.push_back(viral_core::vector(0, -1, 0).normalized());


		// === bottom face === 
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y + size, vec.z - size));	//20, near_bot_left
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y + size, vec.z - size));	//21, near_bot_right
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x - size, vec.y + size, vec.z + size));	//22, far_bot_left
		geometry_mesh->vertices().push_back(viral_core::vector(vec.x + size, vec.y + size, vec.z + size));	//23, far_bot_right

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 20, start_index + 22, start_index + 21));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 21, start_index + 22, start_index + 23));

		normals.push_back(viral_core::vector(0, 1, 0).normalized());
		normals.push_back(viral_core::vector(0, 1, 0).normalized());
		normals.push_back(viral_core::vector(0, 1, 0).normalized());
		normals.push_back(viral_core::vector(0, 1, 0).normalized());
	}







	void addBoxWithCorners(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh,
		viral_core::vector near_top_left, viral_core::vector near_top_right, viral_core::vector near_bot_left, viral_core::vector near_bot_right,
		viral_core::vector far_top_left, viral_core::vector far_top_right, viral_core::vector far_bot_left, viral_core::vector far_bot_right) {

		int start_index = geometry_mesh->vertex_count();

		viral_core::mesh::const_stream_iterator normals_it = geometry_mesh->find_vertex_stream(viral_core::mesh_stream_registry::normal_stream_name);
		if (!normals_it) {
			viral_core::auto_pointer<viral_core::mesh_stream_vector> new_normals(new viral_core::mesh_stream_vector());
			normals_it = geometry_mesh->insert_vertex_stream(viral_core::mesh_stream_registry::normal_stream_name, new_normals);
		}
		viral_core::mesh_stream_vector& normals = static_cast<viral_core::mesh_stream_vector&>(*normals_it->value);



		// === front face ===
		geometry_mesh->vertices().push_back(viral_core::vector(near_top_left));		//0, near_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(near_top_right));	//1, near_top_right
		geometry_mesh->vertices().push_back(viral_core::vector(near_bot_left));		//2, near_bot_left
		geometry_mesh->vertices().push_back(viral_core::vector(near_bot_right));	//3, near_bot_right

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 0, start_index + 2, start_index + 1));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 0, start_index + 2, start_index + 1));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 1, start_index + 2, start_index + 3));

		normals.push_back(viral_core::vector(0, 0, -1).normalized());
		normals.push_back(viral_core::vector(0, 0, -1).normalized());
		normals.push_back(viral_core::vector(0, 0, -1).normalized());
		normals.push_back(viral_core::vector(0, 0, -1).normalized());


		// === right face ===
		geometry_mesh->vertices().push_back(viral_core::vector(near_top_right));	//4, near_top_right
		geometry_mesh->vertices().push_back(viral_core::vector(far_top_right));		//5, far_top_right
		geometry_mesh->vertices().push_back(viral_core::vector(near_bot_right));	//6, near_bot_right
		geometry_mesh->vertices().push_back(viral_core::vector(far_bot_right));		//7, far_bot_right

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 4, start_index + 6, start_index + 5));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 5, start_index + 6, start_index + 7));

		normals.push_back(viral_core::vector(1, 0, 0).normalized());
		normals.push_back(viral_core::vector(1, 0, 0).normalized());
		normals.push_back(viral_core::vector(1, 0, 0).normalized());
		normals.push_back(viral_core::vector(1, 0, 0).normalized());


		// === back face === 
		geometry_mesh->vertices().push_back(viral_core::vector(far_top_right));		//8, far_top_right
		geometry_mesh->vertices().push_back(viral_core::vector(far_top_left));		//9, far_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(far_bot_right));		//10, far_bot_right
		geometry_mesh->vertices().push_back(viral_core::vector(far_bot_left));		//11, far_bot_left

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 8, start_index + 10, start_index + 9));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 9, start_index + 10, start_index + 11));

		normals.push_back(viral_core::vector(0, 0, 1).normalized());
		normals.push_back(viral_core::vector(0, 0, 1).normalized());
		normals.push_back(viral_core::vector(0, 0, 1).normalized());
		normals.push_back(viral_core::vector(0, 0, 1).normalized());


		// === left face === 
		geometry_mesh->vertices().push_back(viral_core::vector(far_top_left));		//12, far_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(near_top_left));		//13, near_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(far_bot_left));		//14, far_bot_left
		geometry_mesh->vertices().push_back(viral_core::vector(near_bot_left));		//15, near_bot_left

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 12, start_index + 14, start_index + 13));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 13, start_index + 14, start_index + 15));

		normals.push_back(viral_core::vector(-1, 0, 0).normalized());
		normals.push_back(viral_core::vector(-1, 0, 0).normalized());
		normals.push_back(viral_core::vector(-1, 0, 0).normalized());
		normals.push_back(viral_core::vector(-1, 0, 0).normalized());


		// === top face === 
		geometry_mesh->vertices().push_back(viral_core::vector(far_top_left));		//16, far_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(far_top_right));		//17, far_top_right
		geometry_mesh->vertices().push_back(viral_core::vector(near_top_left));		//18, near_top_left
		geometry_mesh->vertices().push_back(viral_core::vector(near_top_right));	//19, near_top_right

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 16, start_index + 18, start_index + 17));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 17, start_index + 18, start_index + 19));

		normals.push_back(viral_core::vector(0, -1, 0).normalized());
		normals.push_back(viral_core::vector(0, -1, 0).normalized());
		normals.push_back(viral_core::vector(0, -1, 0).normalized());
		normals.push_back(viral_core::vector(0, -1, 0).normalized());


		// === bottom face === 
		geometry_mesh->vertices().push_back(viral_core::vector(near_bot_left));		//20, near_bot_left
		geometry_mesh->vertices().push_back(viral_core::vector(near_bot_right));	//21, near_bot_right
		geometry_mesh->vertices().push_back(viral_core::vector(far_bot_left));		//22, far_bot_left
		geometry_mesh->vertices().push_back(viral_core::vector(far_bot_right));		//23, far_bot_right

		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 20, start_index + 22, start_index + 21));
		geometry_mesh->triangles().push_back(viral_core::mesh_triangle(start_index + 21, start_index + 22, start_index + 23));

		normals.push_back(viral_core::vector(0, 1, 0).normalized());
		normals.push_back(viral_core::vector(0, 1, 0).normalized());
		normals.push_back(viral_core::vector(0, 1, 0).normalized());
		normals.push_back(viral_core::vector(0, 1, 0).normalized());
	}







	void addCoordinateSystem(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh, float sizeX, float sizeY, float sizeZ) {
		float size = 0.5;

		// x-axis
		viral_core::vector near_top_left(-size, -size, -size);
		viral_core::vector near_top_right(sizeX, -size, -size);
		viral_core::vector near_bot_left(-size, size, -size);
		viral_core::vector near_bot_right(sizeX, size, -size);
		viral_core::vector far_top_left(-size, -size, size);
		viral_core::vector far_top_right(sizeX, -size, size);
		viral_core::vector far_bot_left(-size, size, size);
		viral_core::vector far_bot_right(sizeX, size, size);
		addBoxWithCorners(geometry_mesh, near_top_left, near_top_right, near_bot_left, near_bot_right,
			far_top_left, far_top_right, far_bot_left, far_bot_right);


		// y-axis
		near_top_left = viral_core::vector(-size, -size, -size);
		near_top_right = viral_core::vector(size, -size, -size);
		near_bot_left = viral_core::vector(-size, sizeY, -size);
		near_bot_right = viral_core::vector(size, sizeY, -size);
		far_top_left = viral_core::vector(-size, -size, size);
		far_top_right = viral_core::vector(size, -size, size);
		far_bot_left = viral_core::vector(-size, sizeY, size);
		far_bot_right = viral_core::vector(size, sizeY, size);
		addBoxWithCorners(geometry_mesh, near_top_left, near_top_right, near_bot_left, near_bot_right,
			far_top_left, far_top_right, far_bot_left, far_bot_right);


		// z-axis
		near_top_left = viral_core::vector(-size, -size, -size);
		near_top_right = viral_core::vector(size, -size, -size);
		near_bot_left = viral_core::vector(-size, size, -size);
		near_bot_right = viral_core::vector(size, size, -size);
		far_top_left = viral_core::vector(-size, -size, sizeZ);
		far_top_right = viral_core::vector(size, -size, sizeZ);
		far_bot_left = viral_core::vector(-size, size, sizeZ);
		far_bot_right = viral_core::vector(size, size, sizeZ);
		addBoxWithCorners(geometry_mesh, near_top_left, near_top_right, near_bot_left, near_bot_right,
			far_top_left, far_top_right, far_bot_left, far_bot_right);

	}








}