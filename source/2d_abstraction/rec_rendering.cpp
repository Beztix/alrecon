/**
*************************************************************************
*
* @file rec_trivial_reconstruction.cpp
*
* 
*
************************************************************************/

#include "rec_trivial_reconstruction.h"


#include "image_input.h"
#include "text_input.h"
#include "text_output.h"
#include "util.h"

// Viral
#include <viral_core/engine.hpp>
#include <viral_core/window.hpp>
#include <viral_core/input.hpp>
#include <viral_core/file.hpp>
#include <viral_core/timer.hpp>
#include <viral_core/image.hpp>

#include <viral_core/render_resource.hpp>
#include <viral_core/render_scene.hpp>
#include <viral_core/render_process.hpp>
#include <viral_core/render_command.hpp>
#include <viral_core/render_task.hpp>
#include <viral_core/render.hpp>



#include "rec_file_camera.h"
#include "rec_sensor.h"
#include "rec_sampling.h"

#include <vector>



namespace rec {



	// utility method to update the camera according to user input
	void update_camera
		(viral_core::window_context& window,
		viral_core::input_context& input,
		viral_core::camera& cam)
	{
		using namespace viral_core;
		typedef viral_core::vector vector;

		vector2f window_pos(window.position());
		vector2f window_size(window.size());


		// Update camera transforms from user input.
		bool mouse_in_window =
			input.value_of(input_source::m_absolute_x) >= window_pos.x &&
			input.value_of(input_source::m_absolute_y) >= window_pos.y &&
			input.value_of(input_source::m_absolute_x) <= window_pos.x + window_size.x &&
			input.value_of(input_source::m_absolute_y) <= window_pos.y + window_size.y;

		input.freeze_mouse
			(mouse_in_window &&
			(input.is_active(input_source::m_button0) ||
			input.is_active(input_source::m_button1)));

		if (mouse_in_window) 
		{
			if (input.is_active(input_source::m_button0))
			{
				cam.orientation *=
					rotation(vector(0, 1, 0), input.value_of(input_source::m_axis0) * 0.1f);
				cam.orientation *=
					rotation(vector(1, 0, 0), input.value_of(input_source::m_axis1) * 0.1f);
			}

			cam.orientation *=
				rotation(vector(0, 1, 0), input.value_of(input_source::j0_axis0) * 0.1f);
			cam.orientation *=
				rotation(vector(1, 0, 0), -input.value_of(input_source::j0_axis1) * 0.1f);
			cam.orientation *=
				rotation(vector(0, 0, 1), input.value_of(input_source::j0_button6) * 0.1f);
			cam.orientation *=
				rotation(vector(0, 0, 1), -input.value_of(input_source::j0_button7) * 0.1f);

			vector movement
				(vector(0, 0, 1) * input.value_of(input_source::j0_axis5) +
				vector(0, 0, -1) * input.value_of(input_source::j0_axis4) +
				vector(0, 0, 1) * input.value_of(input_source::k_w) +
				vector(0, 0, -1) * input.value_of(input_source::k_s) +
				vector(1, 0, 0) * input.value_of(input_source::j0_axis2) +
				vector(1, 0, 0) * input.value_of(input_source::k_d) +
				vector(-1, 0, 0) * input.value_of(input_source::k_a) +
				vector(0, 1, 0) * input.value_of(input_source::j0_axis3) +
				vector(0, 1, 0) * input.value_of(input_source::k_space) +
				vector(0, -1, 0) * input.value_of(input_source::k_c));

			if (movement.length() > 0.0001f)
			{
				static const float camera_speed = 2.f;
				movement *= camera_speed;

				if (movement.length() > camera_speed)
					movement = movement.normalized() * camera_speed;

				movement = cam.orientation * movement;
				cam.position += movement;
			}
		}
	}




	void addLine(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh,
		viral_core::vector vec_one, viral_core::vector vec_two) {
	
		int start_index = geometry_mesh->vertex_count();

		viral_core::mesh::const_stream_iterator normals_it = geometry_mesh->find_vertex_stream(viral_core::mesh_stream_registry::normal_stream_name);
		if (!normals_it)
		{
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
		if (!normals_it)
		{
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

		viral_core::mesh::const_stream_iterator normals_it = geometry_mesh->find_vertex_stream
			(viral_core::mesh_stream_registry::normal_stream_name);

		if (!normals_it)
		{
			viral_core::auto_pointer<viral_core::mesh_stream_vector> new_normals
				(new viral_core::mesh_stream_vector());

			normals_it = geometry_mesh->insert_vertex_stream
				(viral_core::mesh_stream_registry::normal_stream_name,
				new_normals);
		}

		viral_core::mesh_stream_vector& normals =
			static_cast<viral_core::mesh_stream_vector&>(*normals_it->value);


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
		viral_core::vector far_top_left, viral_core::vector far_top_right, viral_core::vector far_bot_left, viral_core::vector far_bot_right ) {
	

		int start_index = geometry_mesh->vertex_count();

		viral_core::mesh::const_stream_iterator normals_it = geometry_mesh->find_vertex_stream
			(viral_core::mesh_stream_registry::normal_stream_name); 

		if (!normals_it)
		{
			viral_core::auto_pointer<viral_core::mesh_stream_vector> new_normals
				(new viral_core::mesh_stream_vector());

			normals_it = geometry_mesh->insert_vertex_stream
				(viral_core::mesh_stream_registry::normal_stream_name,
				 new_normals); 
		}

		viral_core::mesh_stream_vector& normals =
			static_cast<viral_core::mesh_stream_vector&>(*normals_it->value);


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








	void renderOccupiedPositions(std::vector<viral_core::vector> occupiedPositions, float cubesize, float scale) {

		int nrOfPuppets = 100;
		int cubesPerPuppet = (int) (occupiedPositions.size() / (nrOfPuppets-1));



		// ######################### Initialization ######################### 


		viral_core::engine e("alrecon");
		viral_core::step_timer t(0.02f, 0.5f);

		// ======= Resources ======= 
		std::vector<viral_core::shared_pointer<viral_core::render_model_id>> model_cubes_ids;
		for (int i = 0; i < nrOfPuppets; i++) {
			viral_core::shared_pointer<viral_core::render_model_id> current_model_id
				(new viral_core::render_model_id("my_model_" + i));
			model_cubes_ids.emplace_back(current_model_id);
		}  
		viral_core::shared_pointer<viral_core::render_model_id> model_boundingBox_id
			(new viral_core::render_model_id("my_boundingBox_model"));
		viral_core::shared_pointer<viral_core::render_model_id> model_coordinateAxes_id
			(new viral_core::render_model_id("my_coordinateAxes_model"));
		viral_core::shared_pointer<viral_core::render_model_id> model_cameraPlanes_id
			(new viral_core::render_model_id("my_cameraPlanes_model"));
		viral_core::shared_pointer<viral_core::render_model_id> model_cameraRays_id
			(new viral_core::render_model_id("my_cameraRays_model"));
		viral_core::shared_pointer<viral_core::render_shader_id> shader_id
			(new viral_core::render_shader_id("my_shader"));
		viral_core::shared_pointer<viral_core::render_material_id> material_id
			(new viral_core::render_material_id("my_material"));
		viral_core::shared_pointer<viral_core::render_material_id> material_wireframe_id
			(new viral_core::render_material_id("my_wireframe_material"));
		viral_core::shared_pointer<viral_core::render_material_id> material_unlit_id
			(new viral_core::render_material_id("my_unlit_material"));
		viral_core::shared_pointer<viral_core::render_material_id> material_transparent_id
			(new viral_core::render_material_id("my_transparent_material"));


		// ======= Scene ======= 
		std::vector<viral_core::shared_pointer<viral_core::render_puppet_id>> puppet_ids;
		for (int i = 0; i < nrOfPuppets; i++) {
			viral_core::shared_pointer<viral_core::render_puppet_id> current_puppet_id
				(new viral_core::render_puppet_id("my_puppet_" + i));
			puppet_ids.emplace_back(current_puppet_id);
		}
		viral_core::shared_pointer<viral_core::render_puppet_id> puppet_boundingBox_id
			(new viral_core::render_puppet_id("my_boundingBox_puppet"));
		viral_core::shared_pointer<viral_core::render_puppet_id> puppet_coordinateAxes_id
			(new viral_core::render_puppet_id("my_coordinateAxes_puppet"));
		viral_core::shared_pointer<viral_core::render_puppet_id> puppet_cameraPlanes_id
			(new viral_core::render_puppet_id("my_cameraPlanes_puppet"));
		viral_core::shared_pointer<viral_core::render_puppet_id> puppet_cameraRays_id
			(new viral_core::render_puppet_id("my_cameraRays_puppet"));
		viral_core::shared_pointer<viral_core::render_light_id> light_id
			(new viral_core::render_light_id("my_light"));
		viral_core::shared_pointer<viral_core::render_light_id> light2_id
			(new viral_core::render_light_id("my_light_2"));
		viral_core::shared_pointer<viral_core::render_scene_id> scene_id
			(new viral_core::render_scene_id("my_scene"));

		// ======= Process ======= 
		viral_core::shared_pointer<viral_core::render_layer_id> layer_id
			(new viral_core::render_layer_id("my_layer"));
		viral_core::shared_pointer<viral_core::render_canvas_id> canvas_id
			(new viral_core::render_canvas_id("my_canvas"));
		viral_core::shared_pointer<viral_core::render_process_id> process_id
			(new viral_core::render_process_id("my_process"));




		// ######################### Put together geometry ######################### 


		//create the camera planes
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh_cameraPlanes
			(new viral_core::mesh());

		//create the camera rays
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh_cameraRays
			(new viral_core::mesh());


		//for each camera
		for (int i = 0; i < 7; i++) {

			file_camera current_cam(i);
			sensor current_sensor(i, current_cam.image_size, sensor::projection_pinhole_distort);
			current_sensor.set_pinhole_distort(current_cam.world_to_device_pinhole_distort_, current_cam.pinhole_distort_center_, current_cam.pinhole_distort_focus_, 
				current_cam.distort_r1_, current_cam.distort_r2_, current_cam.distort_t1_, current_cam.distort_t2_);

			// ==== sample test (whole plane) ==== 

			//sample the plane in front of the camera, receive the corners of the plane
			//std::vector<viral_core::vector> vectors = rec::sample_camera_to_image(current_cam, current_sensor, 500);
			//add the plane in front of the camera to the geometry
			//addSquare(geometry_mesh_testSquare, vectors.at(0), vectors.at(1), vectors.at(2), vectors.at(3));


			// ==== sampling to create files ====
			//rec::sample_camera_for_inv_projection(current_cam, current_sensor, 700);

			


			// ==== load the positions for camera inversion from binary file for distance 500 ====
			std::string locationString = "../../assets/camera_inversion/sampledPositions_d" + std::to_string(500) + "_cam" + std::to_string(i);
			std::vector<std::vector<viral_core::vector>> positionGrid500 = text_input::readPositionsGridFromBinaryfile(locationString + ".bin", 640, 480);

			//add the viewing plane of the camera
			int rasterSize = 5;
			for (int y = 0; y < 480-rasterSize; y+=rasterSize) {
				for (int x = 0; x < 640-rasterSize; x+=rasterSize) {
					addSquare(geometry_mesh_cameraPlanes, positionGrid500.at(y).at(x)*scale, 
						positionGrid500.at(y).at(x + rasterSize)*scale, 
						positionGrid500.at(y + rasterSize).at(x)*scale, 
						positionGrid500.at(y + rasterSize).at(x + rasterSize)*scale);
				}
			}


			
			// ==== load the positions for camera inversion from binary file for distance 700 ====
			locationString = "../../assets/camera_inversion/sampledPositions_d" + std::to_string(700) + "_cam" + std::to_string(i);
			std::vector<std::vector<viral_core::vector>> positionGrid700 = text_input::readPositionsGridFromBinaryfile(locationString + ".bin", 640, 480);

			/*
			//add the viewing plane of the camera
			rasterSize = 5;
			for (int y = 0; y < 480 - rasterSize; y += rasterSize) {
				for (int x = 0; x < 640 - rasterSize; x += rasterSize) {
					addSquare(geometry_mesh_cameraPlanes, positionGrid700.at(y).at(x)*scale,
						positionGrid700.at(y).at(x + rasterSize)*scale,
						positionGrid700.at(y + rasterSize).at(x)*scale,
						positionGrid700.at(y + rasterSize).at(x + rasterSize)*scale);
				}
			}
			*/

			util::float3 startOne;
			util::float3 endOne;
			util::float3 startTwo;
			util::float3 endTwo;

			//add rays
			rasterSize = 100;
			for (int y = 0; y < 480; y += rasterSize) {
				for (int x = 0; x < 640; x += rasterSize) {
					viral_core::vector startVec = positionGrid500.at(y).at(x)*scale;
					viral_core::vector endVec = positionGrid700.at(y).at(x)*scale;

					viral_core::vector direction = endVec - startVec;

					viral_core::vector furtherStartVec = endVec + direction * 3;
					viral_core::vector furtherEndVec = startVec - direction * 3;

					addLine(geometry_mesh_cameraRays, furtherStartVec, furtherEndVec);
					if (y == 0 && x == 0) {
						startOne = util::float3(furtherStartVec.x, furtherStartVec.y, furtherStartVec.z);
						endOne = util::float3(furtherEndVec.x, furtherEndVec.y, furtherEndVec.z);
					}
					if (y == rasterSize && x == rasterSize) {
						startTwo = util::float3(furtherStartVec.x, furtherStartVec.y, furtherStartVec.z);
						endTwo = util::float3(furtherEndVec.x, furtherEndVec.y, furtherEndVec.z);
					}
				}
			}

			//calculate camera position with ray intersection
			util::float3 intersection = util::ClosestPointLineLine(startOne, endOne, startTwo, endTwo);

			//add camera
			addCubeAroundVector(geometry_mesh_cameraPlanes, viral_core::vector(intersection.x, intersection.y, intersection.z), 1.0f);
		}

		viral_core::auto_pointer<viral_core::model> geometry_cameraPlanes
			(new viral_core::model());
		geometry_cameraPlanes->insert_group("model_group_cameraPlanes", geometry_mesh_cameraPlanes);
		geometry_cameraPlanes->rebuild_boundings();
		geometry_cameraPlanes->validate();


		viral_core::auto_pointer<viral_core::model> geometry_cameraRays
			(new viral_core::model());
		geometry_cameraRays->insert_group("model_group_cameraRays", geometry_mesh_cameraRays);
		geometry_cameraRays->rebuild_boundings();
		geometry_cameraRays->validate();




		std::vector<viral_core::auto_pointer<viral_core::model>> geometry_cubes_container;

		//create the geometry model for nrOfPuppets-1 puppets
		for (int i = 0; i < nrOfPuppets-1; i++) {
			viral_core::auto_pointer<viral_core::mesh> current_geometry_mesh(new viral_core::mesh());
			for (int k = i * cubesPerPuppet; k < (i + 1)*cubesPerPuppet; k++) {
				viral_core::vector currentPosition = occupiedPositions.at(k);
				viral_core::vector currentScaledPosition(currentPosition.x * scale, currentPosition.y * scale, currentPosition.z * scale);
				addCubeAroundVector(current_geometry_mesh, currentScaledPosition, cubesize / 10);
			}

			viral_core::auto_pointer<viral_core::model> current_geometry(new viral_core::model());
			current_geometry->insert_group("model_group", current_geometry_mesh);
			current_geometry->rebuild_boundings();
			current_geometry->validate();
			geometry_cubes_container.emplace_back(current_geometry);
		}

		//create the geometry model for the last puppet (separate cause possibly less occupiedPositions than cubesPerPuppet left)
		viral_core::auto_pointer<viral_core::mesh> current_geometry_mesh(new viral_core::mesh());
		for (int k = (nrOfPuppets-1) * cubesPerPuppet; k < occupiedPositions.size(); k++) {
			viral_core::vector currentPosition = occupiedPositions.at(k);
			viral_core::vector currentScaledPosition(currentPosition.x * scale, currentPosition.y * scale, currentPosition.z * scale);
			addCubeAroundVector(current_geometry_mesh, currentScaledPosition, cubesize / 10);
		}
		viral_core::auto_pointer<viral_core::model> current_geometry(new viral_core::model());
		current_geometry->insert_group("model_group", current_geometry_mesh);
		current_geometry->rebuild_boundings();
		current_geometry->validate();
		geometry_cubes_container.emplace_back(current_geometry);


		// create the bounding box of the world space 
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh_boundingBox
			(new viral_core::mesh());

		viral_core::vector near_top_left(-2000 * scale, -2200 * scale, -880 * scale);
		viral_core::vector near_top_right(2200 * scale, -2200 * scale, -880 * scale);
		viral_core::vector near_bot_left(-2000 * scale, 2200 * scale, -880 * scale);
		viral_core::vector near_bot_right(2200 * scale, 2200 * scale, -880 * scale);
		viral_core::vector far_top_left(-2000 * scale, -2200 * scale, 1600 * scale);
		viral_core::vector far_top_right(2200 * scale, -2200 * scale, 1600 * scale);
		viral_core::vector far_bot_left(-2000 * scale, 2200 * scale, 1600 * scale);
		viral_core::vector far_bot_right(2200 * scale, 2200 * scale, 1600 * scale);
		addBoxWithCorners(geometry_mesh_boundingBox, near_top_left, near_top_right, near_bot_left, near_bot_right,
			far_top_left, far_top_right, far_bot_left, far_bot_right);

		viral_core::auto_pointer<viral_core::model> geometry_boundingBox
			(new viral_core::model());
		geometry_boundingBox->insert_group("model_group_boundingBox", geometry_mesh_boundingBox);
		geometry_boundingBox->rebuild_boundings();
		geometry_boundingBox->validate();


		//create the coordinate axes of the world space
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh_coordinateAxes
			(new viral_core::mesh());
		addCoordinateSystem(geometry_mesh_coordinateAxes, 500, 500, 300);

		viral_core::auto_pointer<viral_core::model> geometry_coordinateAxes
			(new viral_core::model());
		geometry_coordinateAxes->insert_group("model_group_coordinateAxes", geometry_mesh_coordinateAxes);
		geometry_coordinateAxes->rebuild_boundings();
		geometry_coordinateAxes->validate();



		// ######################### Fill data structures for later render-side objects ######################### 
		

		//add the geometry models to the render_model_data
		std::vector<viral_core::render_model_data> model_data_cubes_container;
		for (int i = 0; i < nrOfPuppets; i++) {
			viral_core::render_model_data model_data_cube;
			model_data_cube.geometry = geometry_cubes_container.at(i);
			model_data_cubes_container.emplace_back(model_data_cube);
		}
		
		viral_core::render_model_data model_data_boundingBox;
		model_data_boundingBox.geometry = geometry_boundingBox;

		viral_core::render_model_data model_data_cameraPlanes;
		model_data_cameraPlanes.geometry = geometry_cameraPlanes;

		viral_core::render_model_data model_data_cameraRays;
		model_data_cameraRays.geometry = geometry_cameraRays;

		viral_core::render_model_data model_data_coordinateAxes;
		model_data_coordinateAxes.geometry = geometry_coordinateAxes;

		viral_core::render_shader_data shader_data;
		shader_data.fragment_shader =
			e.files().open_file("shaders/shader.glfs", viral_core::file::read_only)->read_text();
		shader_data.vertex_shader =
			e.files().open_file("shaders/shader.glvs", viral_core::file::read_only)->read_text();

		viral_core::render_material_data material_data;
		material_data.ambient_color = viral_core::color(1, 1, 1, 1);
		material_data.diffuse_color = viral_core::color(1, 1, 1, 1);
		material_data.specular_color = viral_core::color(0, 0, 0, 1);
		material_data.cull = viral_core::render_material_data::cull_back;
		material_data.shader = shader_id;

		viral_core::render_material_data material_data_wireframe;
		material_data_wireframe.wireframe = true;
		material_data_wireframe.cull = viral_core::render_material_data::cull_none;
		material_data_wireframe.shader = shader_id;

		viral_core::render_material_data material_data_unlit;
		material_data_unlit.ambient_color = viral_core::color(1, 0.05f, 0.05f, 1);
		material_data_unlit.diffuse_color = viral_core::color(1, 0.05f, 0.05f, 1);
		material_data_unlit.specular_color = viral_core::color(0, 0, 0, 1);
		material_data_unlit.unlit = true;
		material_data_unlit.cull = viral_core::render_material_data::cull_none;
		material_data_unlit.shader = shader_id;

		viral_core::render_material_data material_transparent_data;
		material_transparent_data.ambient_color = viral_core::color(0.05f, 0.5f, 0, 0.2f);
		material_transparent_data.diffuse_color = viral_core::color(0.05f, 0.5f, 0, 0.2f);
		material_transparent_data.specular_color = viral_core::color(0, 0, 0, 0.2f);
		material_transparent_data.blend = viral_core::render_material_data::blend_alpha;
		material_transparent_data.cull = viral_core::render_material_data::cull_none;
		material_transparent_data.shader = shader_id;


		//add the cameraPlanes model to the render_puppet_data
		viral_core::render_puppet_data puppet_data_cameraPlanes;
		puppet_data_cameraPlanes.position = viral_core::vector(0, 0, 0);
		puppet_data_cameraPlanes.model = model_cameraPlanes_id;
		puppet_data_cameraPlanes.materials.insert("model_group_cameraPlanes", material_transparent_id);


		//add the cameraRays model to the render_puppet_data
		viral_core::render_puppet_data puppet_data_cameraRays;
		puppet_data_cameraRays.position = viral_core::vector(0, 0, 0);
		puppet_data_cameraRays.model = model_cameraRays_id;
		puppet_data_cameraRays.materials.insert("model_group_cameraRays", material_transparent_id);


		//add the bounding box model to the render_puppet_data
		viral_core::render_puppet_data puppet_data_boundingBox;
		puppet_data_boundingBox.position = viral_core::vector(0, 0, 0);
		puppet_data_boundingBox.model = model_boundingBox_id;
		puppet_data_boundingBox.materials.insert("model_group_boundingBox", material_wireframe_id);


		//add the coordinate axes model to the render_puppet_data
		viral_core::render_puppet_data puppet_data_coordinateAxes;
		puppet_data_coordinateAxes.position = viral_core::vector(0, 0, 0);
		puppet_data_coordinateAxes.model = model_coordinateAxes_id;
		puppet_data_coordinateAxes.materials.insert("model_group_coordinateAxes", material_unlit_id);



		//add the cube models to the render_puppet_data
		std::vector<viral_core::render_puppet_data> puppet_data_cubes_container;
		for (int i = 0; i < nrOfPuppets; i++) {
			viral_core::render_puppet_data current_puppet_data;
			current_puppet_data.position = viral_core::vector(0, 0, 0);
			current_puppet_data.model = model_cubes_ids.at(i);
			current_puppet_data.materials.insert("model_group", material_id);
			puppet_data_cubes_container.emplace_back(current_puppet_data);
		}

		viral_core::render_light_data light_data;
		light_data.emitter = viral_core::render_light_data::emitter_parallel;
		light_data.ambient_color = viral_core::color(0.05f, 0.3f, 0.05f, 1.f);
		light_data.diffuse_color = viral_core::color(0.1f, 0.7f, 0.1f, 1.f);
		light_data.specular_color = viral_core::color(0, 0, 0, 1.f);
		light_data.intensity = 0.5f;
		light_data.orientation =
			viral_core::rotation(viral_core::vector(1, 0, 0), -30) *
			viral_core::rotation(viral_core::vector(0, 1, 0), 30);

		viral_core::render_light_data light2_data;
		light2_data.emitter = viral_core::render_light_data::emitter_parallel;
		light2_data.ambient_color = viral_core::color(0.05f, 0.05f, 0.3f, 1.f);
		light2_data.diffuse_color = viral_core::color(0.1f, 0.1f, 0.7f, 1.f);
		light2_data.specular_color = viral_core::color(0, 0, 0, 1.f);
		light2_data.intensity = 0.8f;
		light2_data.orientation =
			viral_core::rotation(viral_core::vector(1, 0, 0), -130) *
			viral_core::rotation(viral_core::vector(0, 1, 0), 145);

		//add the puppets to the render_scene_data
		viral_core::render_scene_data scene_data;
		scene_data.objects.insert(puppet_boundingBox_id);
		scene_data.objects.insert(puppet_coordinateAxes_id);
		scene_data.objects.insert(puppet_cameraPlanes_id);
		scene_data.objects.insert(puppet_cameraRays_id);
		for (int i = 0; i < nrOfPuppets; i++) {
			scene_data.objects.insert(puppet_ids[i]);
		}
		scene_data.objects.insert(light_id);
		scene_data.objects.insert(light2_id);

		viral_core::render_layer_data layer_data;
		layer_data.cam.position = viral_core::vector(-500, 0, 0);
		layer_data.cam.orientation =
			viral_core::rotation(viral_core::vector(1, 0, 0), -90) * 
			viral_core::rotation(viral_core::vector(0, 1, 0), 90);
		layer_data.scene = scene_id;
		layer_data.background_color = viral_core::color(0, 0, 0, 0);

		viral_core::render_canvas_data canvas_data;
		canvas_data.layers.insert(layer_id);
		canvas_data.copy_to_window = true;

		viral_core::render_process_data process_data;
		process_data.canvases.insert(canvas_id);
		process_data.show_results = true;




		// ######################### Commit data structures to render-side objects with appropriate ID ######################### 


		viral_core::auto_pointer<viral_core::render_command_queue> q
			(new viral_core::render_command_queue());

		// commit models
		q->commit(model_data_coordinateAxes, model_coordinateAxes_id);
		q->commit(model_data_boundingBox, model_boundingBox_id);
		q->commit(model_data_cameraPlanes, model_cameraPlanes_id);
		q->commit(model_data_cameraRays, model_cameraRays_id);
		for (int i = 0; i < nrOfPuppets; i++)
			q->commit(model_data_cubes_container[i], model_cubes_ids[i]);

		q->commit(shader_data, shader_id);
		q->commit(material_data, material_id);
		q->commit(material_data_wireframe, material_wireframe_id);
		q->commit(material_data_unlit, material_unlit_id);
		q->commit(material_transparent_data, material_transparent_id);
		
		// commit puppets
		q->commit(puppet_data_coordinateAxes, puppet_coordinateAxes_id);
		q->commit(puppet_data_boundingBox, puppet_boundingBox_id);
		q->commit(puppet_data_cameraPlanes, puppet_cameraPlanes_id);
		q->commit(puppet_data_cameraRays, puppet_cameraRays_id);
		for (int i = 0; i < nrOfPuppets; i++)
			q->commit(puppet_data_cubes_container[i], puppet_ids[i]);

		q->commit(light_data, light_id);
		q->commit(light2_data, light2_id);
		q->commit(scene_data, scene_id);
		q->commit(layer_data, layer_id);
		q->commit(canvas_data, canvas_id);
		q->commit(process_data, process_id);
		e.renderer().execute(q);

		e.window().set_display
			(viral_core::vector2f(50, 50), viral_core::vector2f(800, 600), 0);




		// ######################### Render loop ######################### 


		while (true)
		{
			t.try_sleep();
			t.update();

			if (!t.has_next_timestep())
				continue;


			// Logik
			while (t.next_timestep())
			{
				e.window().update();
				e.input().update();

				if (e.window().close_flag())
					return;

				update_camera(e.window(), e.input(), layer_data.cam);

				// Eigener LOGIK-CODE
				// Hier sind (mindestens) t.timestep_seconds vergangen
			}


			// Rendering
			if (!e.rtask().has_pending_queue())
			{
				// Put together render commands.
				viral_core::auto_pointer<viral_core::render_command_queue> q
					(new viral_core::render_command_queue());

				layer_data.cam.aspect_y_to_x =
					e.window().size().y /
					e.window().size().x;
				q->commit(layer_data, layer_id);

				canvas_data.pixel_size =
					canvas_data.target_window_pixels.size =
					e.window().size().to_vector2i();
				q->commit(canvas_data, canvas_id);

				// Eigener RENDERING-CODE
				// Hier sind (mindestens) t.timestep_seconds vergangen

				q->render(process_id);
				e.rtask().execute(q);
			}
		}


	}

}