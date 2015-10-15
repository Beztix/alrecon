/**
*************************************************************************
*
* @file reconstruction_primitive.cpp
*
* 
*
************************************************************************/

#include "reconstruction_primitive.h"


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



namespace reconstruct {



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








	void createCubeAroundVector(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh, viral_core::vector vec, float size) {
		

		//8 corners, each contained 3 times for the three adjacent faces of the cube (each of the three gets a different normal)
		geometry_mesh->allocate_vertices(24);

		//2 triangles for each of the 6 faces of the cube
		geometry_mesh->allocate_triangles(12);

		//one normal for each vertex
		viral_core::auto_pointer<viral_core::mesh_stream_vector> geometry_mesh_normals
			(new viral_core::mesh_stream_vector());
		geometry_mesh_normals->resize(24);


		// === front face ===
		geometry_mesh->vertices()[0] = viral_core::vector(vec.x - size, vec.y - size, vec.z - size);	//near_top_left
		geometry_mesh->vertices()[1] = viral_core::vector(vec.x + size, vec.y - size, vec.z - size);	//near_top_right
		geometry_mesh->vertices()[2] = viral_core::vector(vec.x - size, vec.y + size, vec.z - size);	//near_bot_left
		geometry_mesh->vertices()[3] = viral_core::vector(vec.x + size, vec.y + size, vec.z - size);	//near_bot_right

		geometry_mesh->triangles()[0] = viral_core::mesh_triangle(0, 2, 1);
		geometry_mesh->triangles()[1] = viral_core::mesh_triangle(1, 2, 3);

		(*geometry_mesh_normals)[0] = viral_core::vector(0, 0, -1).normalized();
		(*geometry_mesh_normals)[1] = viral_core::vector(0, 0, -1).normalized();
		(*geometry_mesh_normals)[2] = viral_core::vector(0, 0, -1).normalized();
		(*geometry_mesh_normals)[3] = viral_core::vector(0, 0, -1).normalized();


		// === right face ===
		geometry_mesh->vertices()[4] = viral_core::vector(vec.x + size, vec.y - size, vec.z - size);	//near_top_right
		geometry_mesh->vertices()[5] = viral_core::vector(vec.x + size, vec.y - size, vec.z + size);	//far_top_right
		geometry_mesh->vertices()[6] = viral_core::vector(vec.x + size, vec.y + size, vec.z - size);	//near_bot_right
		geometry_mesh->vertices()[7] = viral_core::vector(vec.x + size, vec.y + size, vec.z + size);	//far_bot_right

		geometry_mesh->triangles()[2] = viral_core::mesh_triangle(4, 6, 5);
		geometry_mesh->triangles()[3] = viral_core::mesh_triangle(5, 6, 7);

		(*geometry_mesh_normals)[4] = viral_core::vector(1, 0, 0).normalized();
		(*geometry_mesh_normals)[5] = viral_core::vector(1, 0, 0).normalized();
		(*geometry_mesh_normals)[6] = viral_core::vector(1, 0, 0).normalized();
		(*geometry_mesh_normals)[7] = viral_core::vector(1, 0, 0).normalized();


		// === back face === 
		geometry_mesh->vertices()[8] = viral_core::vector(vec.x + size, vec.y - size, vec.z + size);	//far_top_right
		geometry_mesh->vertices()[9] = viral_core::vector(vec.x - size, vec.y - size, vec.z + size);	//far_top_left
		geometry_mesh->vertices()[10] = viral_core::vector(vec.x + size, vec.y + size, vec.z + size);	//far_bot_right
		geometry_mesh->vertices()[11] = viral_core::vector(vec.x - size, vec.y + size, vec.z + size);	//far_bot_left

		geometry_mesh->triangles()[4] = viral_core::mesh_triangle(8, 10, 9);
		geometry_mesh->triangles()[5] = viral_core::mesh_triangle(9, 10, 11);

		(*geometry_mesh_normals)[8] = viral_core::vector(0, 0, 1).normalized();
		(*geometry_mesh_normals)[9] = viral_core::vector(0, 0, 1).normalized();
		(*geometry_mesh_normals)[10] = viral_core::vector(0, 0, 1).normalized();
		(*geometry_mesh_normals)[11] = viral_core::vector(0, 0, 1).normalized();


		// === left face === 
		geometry_mesh->vertices()[12] = viral_core::vector(vec.x - size, vec.y - size, vec.z + size);	//far_top_left
		geometry_mesh->vertices()[13] = viral_core::vector(vec.x - size, vec.y - size, vec.z - size);	//near_top_left
		geometry_mesh->vertices()[14] = viral_core::vector(vec.x - size, vec.y + size, vec.z + size);	//far_bot_left
		geometry_mesh->vertices()[15] = viral_core::vector(vec.x - size, vec.y + size, vec.z - size);	//near_bot_left

		geometry_mesh->triangles()[6] = viral_core::mesh_triangle(12, 14, 13);
		geometry_mesh->triangles()[7] = viral_core::mesh_triangle(13, 14, 15);

		(*geometry_mesh_normals)[12] = viral_core::vector(-1, 0, 0).normalized();
		(*geometry_mesh_normals)[13] = viral_core::vector(-1, 0, 0).normalized();
		(*geometry_mesh_normals)[14] = viral_core::vector(-1, 0, 0).normalized();
		(*geometry_mesh_normals)[15] = viral_core::vector(-1, 0, 0).normalized();


		// === top face === 
		geometry_mesh->vertices()[16] = viral_core::vector(vec.x - size, vec.y - size, vec.z + size);	//far_top_left
		geometry_mesh->vertices()[17] = viral_core::vector(vec.x + size, vec.y - size, vec.z + size);	//far_top_right
		geometry_mesh->vertices()[18] = viral_core::vector(vec.x - size, vec.y - size, vec.z - size);	//near_top_left
		geometry_mesh->vertices()[19] = viral_core::vector(vec.x + size, vec.y - size, vec.z - size);	//near_top_right

		geometry_mesh->triangles()[8] = viral_core::mesh_triangle(16, 18, 17);
		geometry_mesh->triangles()[9] = viral_core::mesh_triangle(17, 18, 19);

		(*geometry_mesh_normals)[16] = viral_core::vector(0, -1, 0).normalized();
		(*geometry_mesh_normals)[17] = viral_core::vector(0, -1, 0).normalized();
		(*geometry_mesh_normals)[18] = viral_core::vector(0, -1, 0).normalized();
		(*geometry_mesh_normals)[19] = viral_core::vector(0, -1, 0).normalized();


		// === bottom face === 
		geometry_mesh->vertices()[20] = viral_core::vector(vec.x - size, vec.y + size, vec.z - size);	//near_bot_left
		geometry_mesh->vertices()[21] = viral_core::vector(vec.x + size, vec.y + size, vec.z - size);	//near_bot_right
		geometry_mesh->vertices()[22] = viral_core::vector(vec.x - size, vec.y + size, vec.z + size);	//far_bot_left
		geometry_mesh->vertices()[23] = viral_core::vector(vec.x + size, vec.y + size, vec.z + size);	//far_bot_right

		geometry_mesh->triangles()[10] = viral_core::mesh_triangle(20, 22, 21);
		geometry_mesh->triangles()[11] = viral_core::mesh_triangle(21, 22, 23);

		(*geometry_mesh_normals)[20] = viral_core::vector(0, 1, 0).normalized();
		(*geometry_mesh_normals)[21] = viral_core::vector(0, 1, 0).normalized();
		(*geometry_mesh_normals)[22] = viral_core::vector(0, 1, 0).normalized();
		(*geometry_mesh_normals)[23] = viral_core::vector(0, 1, 0).normalized();


		


		geometry_mesh->insert_vertex_stream
			(viral_core::mesh_stream_registry::normal_stream_name,
			geometry_mesh_normals);



	}









	void createBoxWithCorners(viral_core::auto_pointer<viral_core::mesh> &geometry_mesh, 
		viral_core::vector near_top_left, viral_core::vector near_top_right, viral_core::vector near_bot_left, viral_core::vector near_bot_right, 
		viral_core::vector far_top_left, viral_core::vector far_top_right, viral_core::vector far_bot_left, viral_core::vector far_bot_right ) {


		//8 corners, each contained 3 times for the three adjacent faces of the cube (each of the three gets a different normal)
		geometry_mesh->allocate_vertices(24);

		//2 triangles for each of the 6 faces of the cube
		geometry_mesh->allocate_triangles(12);

		//one normal for each vertex
		viral_core::auto_pointer<viral_core::mesh_stream_vector> geometry_mesh_normals
			(new viral_core::mesh_stream_vector());
		geometry_mesh_normals->resize(24);


		// === front face ===
		geometry_mesh->vertices()[0] = viral_core::vector(near_top_left);	//near_top_left
		geometry_mesh->vertices()[1] = viral_core::vector(near_top_right);	//near_top_right
		geometry_mesh->vertices()[2] = viral_core::vector(near_bot_left);	//near_bot_left
		geometry_mesh->vertices()[3] = viral_core::vector(near_bot_right);	//near_bot_right

		geometry_mesh->triangles()[0] = viral_core::mesh_triangle(0, 2, 1);
		geometry_mesh->triangles()[1] = viral_core::mesh_triangle(1, 2, 3);

		(*geometry_mesh_normals)[0] = viral_core::vector(0, 0, -1).normalized();
		(*geometry_mesh_normals)[1] = viral_core::vector(0, 0, -1).normalized();
		(*geometry_mesh_normals)[2] = viral_core::vector(0, 0, -1).normalized();
		(*geometry_mesh_normals)[3] = viral_core::vector(0, 0, -1).normalized();


		// === right face ===
		geometry_mesh->vertices()[4] = viral_core::vector(near_top_right);	//near_top_right
		geometry_mesh->vertices()[5] = viral_core::vector(far_top_right);	//far_top_right
		geometry_mesh->vertices()[6] = viral_core::vector(near_bot_right);	//near_bot_right
		geometry_mesh->vertices()[7] = viral_core::vector(far_bot_right);	//far_bot_right

		geometry_mesh->triangles()[2] = viral_core::mesh_triangle(4, 6, 5);
		geometry_mesh->triangles()[3] = viral_core::mesh_triangle(5, 6, 7);

		(*geometry_mesh_normals)[4] = viral_core::vector(1, 0, 0).normalized();
		(*geometry_mesh_normals)[5] = viral_core::vector(1, 0, 0).normalized();
		(*geometry_mesh_normals)[6] = viral_core::vector(1, 0, 0).normalized();
		(*geometry_mesh_normals)[7] = viral_core::vector(1, 0, 0).normalized();


		// === back face === 
		geometry_mesh->vertices()[8] = viral_core::vector(far_top_right);	//far_top_right
		geometry_mesh->vertices()[9] = viral_core::vector(far_top_left);	//far_top_left
		geometry_mesh->vertices()[10] = viral_core::vector(far_bot_right);	//far_bot_right
		geometry_mesh->vertices()[11] = viral_core::vector(far_bot_left);	//far_bot_left

		geometry_mesh->triangles()[4] = viral_core::mesh_triangle(8, 10, 9);
		geometry_mesh->triangles()[5] = viral_core::mesh_triangle(9, 10, 11);

		(*geometry_mesh_normals)[8] = viral_core::vector(0, 0, 1).normalized();
		(*geometry_mesh_normals)[9] = viral_core::vector(0, 0, 1).normalized();
		(*geometry_mesh_normals)[10] = viral_core::vector(0, 0, 1).normalized();
		(*geometry_mesh_normals)[11] = viral_core::vector(0, 0, 1).normalized();


		// === left face === 
		geometry_mesh->vertices()[12] = viral_core::vector(far_top_left);	//far_top_left
		geometry_mesh->vertices()[13] = viral_core::vector(near_top_left);	//near_top_left
		geometry_mesh->vertices()[14] = viral_core::vector(far_bot_left);	//far_bot_left
		geometry_mesh->vertices()[15] = viral_core::vector(near_bot_left);	//near_bot_left

		geometry_mesh->triangles()[6] = viral_core::mesh_triangle(12, 14, 13);
		geometry_mesh->triangles()[7] = viral_core::mesh_triangle(13, 14, 15);

		(*geometry_mesh_normals)[12] = viral_core::vector(-1, 0, 0).normalized();
		(*geometry_mesh_normals)[13] = viral_core::vector(-1, 0, 0).normalized();
		(*geometry_mesh_normals)[14] = viral_core::vector(-1, 0, 0).normalized();
		(*geometry_mesh_normals)[15] = viral_core::vector(-1, 0, 0).normalized();


		// === top face === 
		geometry_mesh->vertices()[16] = viral_core::vector(far_top_left);	//far_top_left
		geometry_mesh->vertices()[17] = viral_core::vector(far_top_right);	//far_top_right
		geometry_mesh->vertices()[18] = viral_core::vector(near_top_left);	//near_top_left
		geometry_mesh->vertices()[19] = viral_core::vector(near_top_right);	//near_top_right

		geometry_mesh->triangles()[8] = viral_core::mesh_triangle(16, 18, 17);
		geometry_mesh->triangles()[9] = viral_core::mesh_triangle(17, 18, 19);

		(*geometry_mesh_normals)[16] = viral_core::vector(0, -1, 0).normalized();
		(*geometry_mesh_normals)[17] = viral_core::vector(0, -1, 0).normalized();
		(*geometry_mesh_normals)[18] = viral_core::vector(0, -1, 0).normalized();
		(*geometry_mesh_normals)[19] = viral_core::vector(0, -1, 0).normalized();


		// === bottom face === 
		geometry_mesh->vertices()[20] = viral_core::vector(near_bot_left);	//near_bot_left
		geometry_mesh->vertices()[21] = viral_core::vector(near_bot_right);	//near_bot_right
		geometry_mesh->vertices()[22] = viral_core::vector(far_bot_left);	//far_bot_left
		geometry_mesh->vertices()[23] = viral_core::vector(far_bot_right);	//far_bot_right

		geometry_mesh->triangles()[10] = viral_core::mesh_triangle(20, 22, 21);
		geometry_mesh->triangles()[11] = viral_core::mesh_triangle(21, 22, 23);

		(*geometry_mesh_normals)[20] = viral_core::vector(0, 1, 0).normalized();
		(*geometry_mesh_normals)[21] = viral_core::vector(0, 1, 0).normalized();
		(*geometry_mesh_normals)[22] = viral_core::vector(0, 1, 0).normalized();
		(*geometry_mesh_normals)[23] = viral_core::vector(0, 1, 0).normalized();





		geometry_mesh->insert_vertex_stream
			(viral_core::mesh_stream_registry::normal_stream_name,
			geometry_mesh_normals);



	}












	void reconstruct() {

		file_camera cam_0(0);
		sensor sens_0(0, cam_0.image_size, sensor::projection_pinhole_distort);
		sens_0.set_pinhole_distort(cam_0.world_to_device_pinhole_distort_, cam_0.pinhole_distort_center_, cam_0.pinhole_distort_focus_, 
								   cam_0.distort_r1_, cam_0.distort_r2_, cam_0.distort_t1_, cam_0.distort_t2_);

		file_camera cam_1(1);
		sensor sens_1(1, cam_1.image_size, sensor::projection_pinhole_distort);
		sens_1.set_pinhole_distort(cam_1.world_to_device_pinhole_distort_, cam_1.pinhole_distort_center_, cam_1.pinhole_distort_focus_,
			cam_1.distort_r1_, cam_1.distort_r2_, cam_1.distort_t1_, cam_1.distort_t2_);

		file_camera cam_2(2);
		sensor sens_2(2, cam_2.image_size, sensor::projection_pinhole_distort);
		sens_2.set_pinhole_distort(cam_2.world_to_device_pinhole_distort_, cam_2.pinhole_distort_center_, cam_2.pinhole_distort_focus_,
			cam_2.distort_r1_, cam_2.distort_r2_, cam_2.distort_t1_, cam_2.distort_t2_);

		file_camera cam_3(3);
		sensor sens_3(3, cam_3.image_size, sensor::projection_pinhole_distort);
		sens_3.set_pinhole_distort(cam_3.world_to_device_pinhole_distort_, cam_3.pinhole_distort_center_, cam_3.pinhole_distort_focus_,
			cam_3.distort_r1_, cam_3.distort_r2_, cam_3.distort_t1_, cam_3.distort_t2_);

		file_camera cam_4(4);
		sensor sens_4(4, cam_4.image_size, sensor::projection_pinhole_distort);
		sens_4.set_pinhole_distort(cam_4.world_to_device_pinhole_distort_, cam_4.pinhole_distort_center_, cam_4.pinhole_distort_focus_,
			cam_4.distort_r1_, cam_4.distort_r2_, cam_4.distort_t1_, cam_4.distort_t2_);

		file_camera cam_5(5);
		sensor sens_5(5, cam_5.image_size, sensor::projection_pinhole_distort);
		sens_5.set_pinhole_distort(cam_5.world_to_device_pinhole_distort_, cam_5.pinhole_distort_center_, cam_5.pinhole_distort_focus_,
			cam_5.distort_r1_, cam_5.distort_r2_, cam_5.distort_t1_, cam_5.distort_t2_);

		file_camera cam_6(6);
		sensor sens_6(6, cam_6.image_size, sensor::projection_pinhole_distort);
		sens_6.set_pinhole_distort(cam_6.world_to_device_pinhole_distort_, cam_6.pinhole_distort_center_, cam_6.pinhole_distort_focus_,
			cam_6.distort_r1_, cam_6.distort_r2_, cam_6.distort_t1_, cam_6.distort_t2_);


		int width, height;

		int* pixels_0 = image_input::loadPixelsFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_1.png", width, height);
		int* pixels_1 = image_input::loadPixelsFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_2.png", width, height);
		int* pixels_2 = image_input::loadPixelsFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_3.png", width, height);
		int* pixels_3 = image_input::loadPixelsFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_4.png", width, height);
		int* pixels_4 = image_input::loadPixelsFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_5.png", width, height);
		int* pixels_5 = image_input::loadPixelsFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_6.png", width, height);
		int* pixels_6 = image_input::loadPixelsFromImage("../../assets/TeletubbiesStefan/segmentedScenes/occMask_7.png", width, height);




		std::vector<viral_core::vector> occupiedPositions;


		for (int x = -2000; x < 2200; x+=50) {
			for (int y = -2200; y < 2200; y+=50) {
				for (int z = -880; z < 1600; z+=50) {
					viral_core::vector vec(x, y, z);
					viral_core::vector projVec_0 = sens_0.project(vec);
					viral_core::vector projVec_1 = sens_1.project(vec);
					viral_core::vector projVec_2 = sens_2.project(vec);
					viral_core::vector projVec_3 = sens_3.project(vec);
					viral_core::vector projVec_4 = sens_4.project(vec);
					viral_core::vector projVec_5 = sens_5.project(vec);
					viral_core::vector projVec_6 = sens_6.project(vec);

					int x0 = (int)projVec_0.x;
					int y0 = (int)projVec_0.y;
					int x1 = (int)projVec_1.x;
					int y1 = (int)projVec_1.y;
					int x2 = (int)projVec_2.x;
					int y2 = (int)projVec_2.y;
					int x3 = (int)projVec_3.x;
					int y3 = (int)projVec_3.y;
					int x4 = (int)projVec_4.x;
					int y4 = (int)projVec_4.y;
					int x5 = (int)projVec_5.x;
					int y5 = (int)projVec_5.y;
					int x6 = (int)projVec_6.x;
					int y6 = (int)projVec_6.y;

					bool initial = true;
					bool set_0 = initial;
					bool set_1 = initial;
					bool set_2 = initial;
					bool set_3 = initial;
					bool set_4 = initial;
					bool set_5 = initial;
					bool set_6 = initial;
					

					if (0 < x0 && x0 < width && 0 < y0 && y0 < height) {
						set_0 = false;
						if (pixels_0[(int)projVec_0.y*width + (int)projVec_0.x] != 0)
							set_0 = true;
					}
					if (0 < x1 && x1 < width && 0 < y1 && y1 < height) {
						set_1 = false;
						if (pixels_1[(int)projVec_1.y*width + (int)projVec_1.x] != 0)
							set_1 = true;
					}
					if (0 < x2 && x2 < width && 0 < y2 && y2 < height) {
						set_2 = false;
						if (pixels_2[(int)projVec_2.y*width + (int)projVec_2.x] != 0)
							set_2 = true;
					}
					if (0 < x3 && x3 < width && 0 < y3 && y3 < height) {
						set_3 = false;
						if (pixels_3[(int)projVec_3.y*width + (int)projVec_3.x] != 0)
							set_3 = true;
					}
					if (0 < x4 && x4 < width && 0 < y4 && y4 < height) {
						set_4 = false;
						if (pixels_4[(int)projVec_4.y*width + (int)projVec_4.x] != 0)
							set_4 = true;
					}
					if (0 < x5 && x5 < width && 0 < y5 && y5 < height) {
						set_5 = false;
						if (pixels_5[(int)projVec_5.y*width + (int)projVec_5.x] != 0)
							set_5 = true;
					}
					if (0 < x6 && x6 < width && 0 < y6 && y6 < height) {
						set_6 = false;
						if (pixels_6[(int)projVec_6.y*width + (int)projVec_6.x] != 0)
							set_6 = true;
					}

					




					if (set_0 && set_1 && set_2 && set_3 && set_4 && set_5 && set_6) {
						occupiedPositions.emplace_back(vec);
					}
				}
			}
		}
		

		delete [] pixels_0;
		delete [] pixels_1;
		delete [] pixels_2;
		delete [] pixels_3;
		delete [] pixels_4;
		delete [] pixels_5;
		delete [] pixels_6;




		///////// START RENDERING-DEMO

		viral_core::engine e("alrecon");
		viral_core::step_timer t(0.02f, 0.5f);


		// Resourcen
		viral_core::shared_pointer<viral_core::render_model_id> model_id_cube
			(new viral_core::render_model_id("my_model_cube"));
		viral_core::shared_pointer<viral_core::render_model_id> model_id_boundingBox
			(new viral_core::render_model_id("my_model_boundingBox"));
		viral_core::shared_pointer<viral_core::render_shader_id> shader_id
			(new viral_core::render_shader_id("my_shader"));
		viral_core::shared_pointer<viral_core::render_material_id> material_id
			(new viral_core::render_material_id("my_material"));
		viral_core::shared_pointer<viral_core::render_material_id> material_id_wireframe
			(new viral_core::render_material_id("my_material_wireframe"));

		// Scene
		viral_core::shared_pointer<viral_core::render_puppet_id> puppet_id_boundingBox
			(new viral_core::render_puppet_id("my_puppet"));
		std::vector<viral_core::shared_pointer<viral_core::render_puppet_id>> puppet_ids;
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

		// cube
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh_cube
			(new viral_core::mesh());
		
		viral_core::vector testVec(0, 0, 0);
		createCubeAroundVector(geometry_mesh_cube, testVec, 3);

		viral_core::auto_pointer<viral_core::model> geometry_cube
			(new viral_core::model());
		geometry_cube->insert_group("model_group", geometry_mesh_cube);
		geometry_cube->rebuild_boundings();
		geometry_cube->validate();


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
		createBoxWithCorners(geometry_mesh_boundingBox, near_top_left, near_top_right, near_bot_left, near_bot_right,
			far_top_left, far_top_right, far_bot_left, far_bot_right);

		viral_core::auto_pointer<viral_core::model> geometry_boundingBox
			(new viral_core::model());
		geometry_boundingBox->insert_group("model_group_boundingBox", geometry_mesh_boundingBox);
		geometry_boundingBox->rebuild_boundings();
		geometry_boundingBox->validate();


		// ======== Fill data structures for later render-side objects  ======== 
		viral_core::render_model_data model_data_cube;
		model_data_cube.geometry = geometry_cube;

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

		std::vector<viral_core::render_puppet_data> puppet_datas;
		for (int i = 0; i < occupiedPositions.size(); i++) {
			viral_core::shared_pointer<viral_core::render_puppet_id> current_puppet_id
				(new viral_core::render_puppet_id("my_puppet_" + i));
			viral_core::render_puppet_data current_puppet_data;
			viral_core::vector positionVec(occupiedPositions.at(i).x/10, occupiedPositions.at(i).y/10, occupiedPositions.at(i).z/10);
			current_puppet_data.position = positionVec;
			current_puppet_data.model = model_id_cube;
			current_puppet_data.materials.insert("model_group", material_id);
			puppet_ids.emplace_back(current_puppet_id);
			puppet_datas.emplace_back(current_puppet_data);
		}
		
		viral_core::render_light_data light_data;
		light_data.emitter = viral_core::render_light_data::emitter_parallel;
		light_data.orientation =
			viral_core::rotation(viral_core::vector(1, 0, 0), -30) *
			viral_core::rotation(viral_core::vector(0, 1, 0), 30);

		viral_core::render_scene_data scene_data;
		scene_data.objects.insert(puppet_id_boundingBox);
		for (int i = 0; i < occupiedPositions.size(); i++) {
			scene_data.objects.insert(puppet_ids[i]);
		}
		scene_data.objects.insert(light_id);

		viral_core::render_layer_data layer_data;
		layer_data.scene = scene_id;
		layer_data.background_color = viral_core::color(0.02, 0, 0, 0);

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
		q->commit(model_data_cube, model_id_cube);
		q->commit(shader_data, shader_id);
		q->commit(material_data, material_id);
		q->commit(material_data_wireframe, material_id_wireframe);
		q->commit(puppet_data_boundingBox, puppet_id_boundingBox);
		for (int i = 0; i < occupiedPositions.size(); i++) {
			q->commit(puppet_datas[i], puppet_ids[i]);
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