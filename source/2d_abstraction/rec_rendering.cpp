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



#include "file_camera.h"
#include "sensor.h"

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












	void renderOccupiedPositions(std::vector<viral_core::vector> occupiedPositions, float cubesize) {

		int nrOfPuppets = 100;
		int cubesPerPuppet = (int)occupiedPositions.size() / nrOfPuppets;


		///////// START RENDERING-DEMO

		viral_core::engine e("alrecon");
		viral_core::step_timer t(0.02f, 0.5f);


		// Resourcen
		std::vector<viral_core::shared_pointer<viral_core::render_model_id>> model_ids_cubes;
		for (int i = 0; i < nrOfPuppets; i++) {
			viral_core::shared_pointer<viral_core::render_model_id> current_model_id
				(new viral_core::render_model_id("my_model_" + i));
			model_ids_cubes.emplace_back(current_model_id);
		}  
		viral_core::shared_pointer<viral_core::render_model_id> model_id_boundingBox
			(new viral_core::render_model_id("my_model_boundingBox"));
		viral_core::shared_pointer<viral_core::render_shader_id> shader_id
			(new viral_core::render_shader_id("my_shader"));
		viral_core::shared_pointer<viral_core::render_material_id> material_id
			(new viral_core::render_material_id("my_material"));
		viral_core::shared_pointer<viral_core::render_material_id> material_id_wireframe
			(new viral_core::render_material_id("my_material_wireframe"));

		// Scene
		std::vector<viral_core::shared_pointer<viral_core::render_puppet_id>> puppet_ids;
		for (int i = 0; i < nrOfPuppets; i++) {
			viral_core::shared_pointer<viral_core::render_puppet_id> current_puppet_id
				(new viral_core::render_puppet_id("my_puppet_" + i));
			puppet_ids.emplace_back(current_puppet_id);
		}
		viral_core::shared_pointer<viral_core::render_puppet_id> puppet_id_boundingBox
			(new viral_core::render_puppet_id("my_puppet"));
		viral_core::shared_pointer<viral_core::render_light_id> light_id
			(new viral_core::render_light_id("my_light"));
		viral_core::shared_pointer<viral_core::render_scene_id> scene_id
			(new viral_core::render_scene_id("my_scene"));

		// Process
		viral_core::shared_pointer<viral_core::render_layer_id> layer_id
			(new viral_core::render_layer_id("my_layer"));
		viral_core::shared_pointer<viral_core::render_canvas_id> canvas_id
			(new viral_core::render_canvas_id("my_canvas"));
		viral_core::shared_pointer<viral_core::render_process_id> process_id
			(new viral_core::render_process_id("my_process"));



		// ======== Put together geometry ======== 

	


		std::vector<viral_core::auto_pointer<viral_core::model>> geometry_cubes_container;

		for (int i = 0; i < nrOfPuppets; i++) {
			viral_core::auto_pointer<viral_core::mesh> current_geometry_mesh
				(new viral_core::mesh());
			for (int k = i * cubesPerPuppet; k < (i + 1)*cubesPerPuppet; k++) {
				viral_core::vector positionVec(occupiedPositions.at(k).x / 10, occupiedPositions.at(k).y / 10, occupiedPositions.at(k).z / 10);
				addCubeAroundVector(current_geometry_mesh, positionVec, cubesize / 10);
			}

			viral_core::auto_pointer<viral_core::model> current_geometry
				(new viral_core::model());

			current_geometry->insert_group("model_group", current_geometry_mesh);
			current_geometry->rebuild_boundings();
			current_geometry->validate();

			geometry_cubes_container.emplace_back(current_geometry);
		}

	


		// box
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh_boundingBox
			(new viral_core::mesh());

		viral_core::vector near_top_left(-200, -220, -88);
		viral_core::vector near_top_right(220, -220, -88);
		viral_core::vector near_bot_left(-200, 220, -88);
		viral_core::vector near_bot_right(220, 220, -88);
		viral_core::vector far_top_left(-200, -220, 160);
		viral_core::vector far_top_right(220, -220, 160);
		viral_core::vector far_bot_left(-200, 220, 160);
		viral_core::vector far_bot_right(220, 220, 160);
		addBoxWithCorners(geometry_mesh_boundingBox, near_top_left, near_top_right, near_bot_left, near_bot_right,
			far_top_left, far_top_right, far_bot_left, far_bot_right);

		viral_core::auto_pointer<viral_core::model> geometry_boundingBox
			(new viral_core::model());
		geometry_boundingBox->insert_group("model_group_boundingBox", geometry_mesh_boundingBox);
		geometry_boundingBox->rebuild_boundings();
		geometry_boundingBox->validate();



		// ======== Fill data structures for later render-side objects  ======== 
		
		std::vector<viral_core::render_model_data> model_data_cubes_container;

		for (int i = 0; i < nrOfPuppets; i++) {
			viral_core::render_model_data model_data_cube;
			model_data_cube.geometry = geometry_cubes_container.at(i);
			model_data_cubes_container.emplace_back(model_data_cube);
		}
		
		viral_core::render_model_data model_data_boundingBox;
		model_data_boundingBox.geometry = geometry_boundingBox;

		viral_core::render_shader_data shader_data;
		shader_data.fragment_shader =
			e.files().open_file("shaders/shader.glfs", viral_core::file::read_only)->read_text();
		shader_data.vertex_shader =
			e.files().open_file("shaders/shader.glvs", viral_core::file::read_only)->read_text();

		viral_core::render_material_data material_data;
		material_data.ambient_color = viral_core::color(0, 0, 0, 1);
		material_data.diffuse_color = viral_core::color(0, 1, 1, 1);
		material_data.cull = viral_core::render_material_data::cull_none;
		material_data.shader = shader_id;

		viral_core::render_material_data material_data_wireframe;
		material_data_wireframe.wireframe = true;
		material_data_wireframe.cull = viral_core::render_material_data::cull_none;
		material_data_wireframe.shader = shader_id;

		viral_core::render_puppet_data puppet_data_boundingBox;
		puppet_data_boundingBox.position = viral_core::vector(0, 0, 0);
		puppet_data_boundingBox.model = model_id_boundingBox;
		puppet_data_boundingBox.materials.insert("model_group_boundingBox", material_id_wireframe);

		std::vector<viral_core::render_puppet_data> puppet_data_cubes_container;

		for (int i = 0; i < nrOfPuppets; i++) {
			viral_core::render_puppet_data current_puppet_data;
			current_puppet_data.position = viral_core::vector(0, 0, 0);
			current_puppet_data.model = model_ids_cubes.at(i);
			current_puppet_data.materials.insert("model_group", material_id);
			puppet_data_cubes_container.emplace_back(current_puppet_data);
		}

		viral_core::render_light_data light_data;
		light_data.emitter = viral_core::render_light_data::emitter_parallel;
		light_data.orientation =
			viral_core::rotation(viral_core::vector(1, 0, 0), -30) *
			viral_core::rotation(viral_core::vector(0, 1, 0), 30);

		viral_core::render_scene_data scene_data;
		scene_data.objects.insert(puppet_id_boundingBox);
		for (int i = 0; i < nrOfPuppets; i++) {
			scene_data.objects.insert(puppet_ids[i]);
		}
		scene_data.objects.insert(light_id);

		viral_core::render_layer_data layer_data;
		layer_data.scene = scene_id;
		layer_data.background_color = viral_core::color(0.02f, 0, 0, 0);

		viral_core::render_canvas_data canvas_data;
		canvas_data.layers.insert(layer_id);
		canvas_data.copy_to_window = true;

		viral_core::render_process_data process_data;
		process_data.canvases.insert(canvas_id);
		process_data.show_results = true;






















		// ======== Commit data structures to render-side objects with appropriate ID  ======== 
		viral_core::auto_pointer<viral_core::render_command_queue> q
			(new viral_core::render_command_queue());
		q->commit(model_data_boundingBox, model_id_boundingBox);
		for (int i = 0; i < nrOfPuppets; i++) {
			q->commit(model_data_cubes_container[i], model_ids_cubes[i]);
		}
		q->commit(shader_data, shader_id);
		q->commit(material_data, material_id);
		q->commit(material_data_wireframe, material_id_wireframe);
		
		q->commit(puppet_data_boundingBox, puppet_id_boundingBox);
		for (int i = 0; i < nrOfPuppets; i++) {
			q->commit(puppet_data_cubes_container[i], puppet_ids[i]);
		}
		
		q->commit(light_data, light_id);
		q->commit(scene_data, scene_id);
		q->commit(layer_data, layer_id);
		q->commit(canvas_data, canvas_id);
		q->commit(process_data, process_id);
		e.renderer().execute(q);

		e.window().set_display
			(viral_core::vector2f(50, 50), viral_core::vector2f(800, 600), 0);

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





		///////// END RENDERING-DEMO


	}

}