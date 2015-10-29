/**
*************************************************************************
*
* @file rec_rendering.cpp
*
* 
*
************************************************************************/

#include "rec_rendering.h"


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
#include "rec_rendering_util.h"

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
		viral_core::shared_pointer<viral_core::render_model_id> model_cameraDrawings2D_id
			(new viral_core::render_model_id("my_cameraDrawings2D_model"));
		viral_core::shared_pointer<viral_core::render_model_id> model_frustums_id
			(new viral_core::render_model_id("my_frustums_model"));
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
		viral_core::shared_pointer<viral_core::render_material_id> material_transparent2_id
			(new viral_core::render_material_id("my_transparent2_material"));


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
		viral_core::shared_pointer<viral_core::render_puppet_id> puppet_cameraDrawings2D_id
			(new viral_core::render_puppet_id("my_cameraDrawings2D_puppet"));
		viral_core::shared_pointer<viral_core::render_puppet_id> puppet_frustums_id
			(new viral_core::render_puppet_id("my_frustums_puppet"));
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




		//create the camera planes
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh_cameraPlanes
			(new viral_core::mesh());

		//create the camera rays
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh_cameraRays
			(new viral_core::mesh());

		//create the 2d drawings on the camera planes
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh_cameraDrawings2D
			(new viral_core::mesh());

		//create the frustums
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh_frustums
			(new viral_core::mesh());


		//for each camera
		for (int i = 0; i < 7; i++) {

			file_camera current_cam(i);
			sensor current_sensor(i, current_cam.image_size, sensor::projection_pinhole_distort);
			current_sensor.set_pinhole_distort(current_cam.world_to_device_pinhole_distort_, current_cam.pinhole_distort_center_, current_cam.pinhole_distort_focus_, 
				current_cam.distort_r1_, current_cam.distort_r2_, current_cam.distort_t1_, current_cam.distort_t2_);

			viral_core::vector scaledCamPosition = current_cam.cam_position_*scale;

			//add camera
			addCubeAroundVector(geometry_mesh_cameraPlanes, scaledCamPosition, 2.0f);
			


			// ========    viewing plane d = 500    ========

			/*
			std::string locationString = "../../assets/camera_inversion/sampledPositions_d" + std::to_string(500) + "_cam" + std::to_string(i);
			std::vector<std::vector<viral_core::vector>> positionGrid500 = text_input::readPositionsGridFromBinaryfile(locationString + ".bin", 640, 480);
			//add the viewing plane of the camera
			int rasterSize = 10;
			for (int y = 0; y < 480-rasterSize; y+=rasterSize) {
				for (int x = 0; x < 640-rasterSize; x+=rasterSize) {
					addSquare(geometry_mesh_cameraPlanes, positionGrid500.at(y).at(x)*scale, 
						positionGrid500.at(y).at(x + rasterSize)*scale, 
						positionGrid500.at(y + rasterSize).at(x)*scale, 
						positionGrid500.at(y + rasterSize).at(x + rasterSize)*scale);
				}
			}
			*/
			
			if (i == 5){

			// ========    (extended) viewing plane d = 700    ========

			std::string locationString700 = "../../assets/camera_inversion/offset_300/sampledPositions_d" + std::to_string(700) + "_cam" + std::to_string(i);
			std::vector<std::vector<viral_core::vector>> positionGrid700 = text_input::readPositionsGridFromBinaryfile(locationString700 + ".bin", 1240, 1080);
			//add the extended viewing plane of the camera
			int rasterSize700 = 20;
			for (int y = 0; y < 1080 - rasterSize700; y += rasterSize700) {
				for (int x = 0; x < 1240 - rasterSize700; x += rasterSize700) {
					addSquare(geometry_mesh_cameraPlanes, positionGrid700.at(y).at(x)*scale,
						positionGrid700.at(y).at(x + rasterSize700)*scale,
						positionGrid700.at(y + rasterSize700).at(x)*scale,
						positionGrid700.at(y + rasterSize700).at(x + rasterSize700)*scale);
				}
			}
			//add the real viewing plane of the camera
			for (int y = 300; y < 780 - rasterSize700; y += rasterSize700) {
				for (int x = 300; x < 940 - rasterSize700; x += rasterSize700) {
					addSquare(geometry_mesh_cameraPlanes, positionGrid700.at(y).at(x)*scale,
						positionGrid700.at(y).at(x + rasterSize700)*scale,
						positionGrid700.at(y + rasterSize700).at(x)*scale,
						positionGrid700.at(y + rasterSize700).at(x + rasterSize700)*scale);
				}
			}

			

			// ========    viewing rays    ========

			std::string locationStringDirections = "../../assets/camera_inversion/offset_300/directions_distanceNormalized_cam" + std::to_string(i);
			std::vector<std::vector<viral_core::vector>> directionsGrid = text_input::readPositionsGridFromBinaryfile(locationStringDirections + ".bin", 1240, 1080);
			/*
			//add all possible rays
			int rasterSizeAllRays = 50;
			for (int y = 300; y < 780; y += rasterSizeAllRays) {
				for (int x = 300; x < 940; x += rasterSizeAllRays) {
					viral_core::vector direction = directionsGrid.at(y).at(x);
					viral_core::vector endVec = scaledCamPosition + (direction * 100);

					addLine(geometry_mesh_cameraRays, scaledCamPosition, endVec);
				}
			}
			*/
			
			/*
			//add rays corresponding to occupied pixels
			std::string locationStringImage = "BB_occMask_" + std::to_string(i + 1) + "_lvl1.png";
			int width, height;
			int* pixels = image_input::loadPixelGridFromImage(locationStringImage, width, height);
		
			int rasterSizeOccupiedRays = 1;
			for (int y = 0; y < 1080; y += rasterSizeOccupiedRays) {
				for (int x = 0; x < 1240; x += rasterSizeOccupiedRays) {
					int pixelValue = pixels[y*width + x];
					if (pixelValue > 0) {
						viral_core::vector direction = directionsGrid.at(y).at(x);
						viral_core::vector endVec = scaledCamPosition + (direction * 100);

						addLine(geometry_mesh_cameraRays, scaledCamPosition, endVec);
					}
				}
			}
			*/



			// ========    frustums    ========


			std::string locationStringCorners = "BBCorners_occMask_" + std::to_string(i + 1) + "_lvl1.txt";
			std::vector<int> corners = text_input::readIntVectorFromTextfile(locationStringCorners);

			//add lines in the viewing plane corresponding to the 2d bounding box pixels
			std::vector<int> lineData = util::createBoundingBoxLinesFromCorners(corners);
			for (int i = 0; i < lineData.size() / 4; i++) {
				int x1 = lineData[4 * i];
				int y1 = lineData[4 * i + 1];
				int x2 = lineData[4 * i + 2];
				int y2 = lineData[4 * i + 3];

				viral_core::vector direction1 = directionsGrid.at(y1).at(x1);
				viral_core::vector endVec1 = scaledCamPosition + (direction1 * 70);

				viral_core::vector direction2 = directionsGrid.at(y2).at(x2);
				viral_core::vector endVec2 = scaledCamPosition + (direction2 * 70);

				addLine(geometry_mesh_cameraDrawings2D, endVec1, endVec2);
			}

			
	
				//add frustums
				int nrOfBoxes = corners.size() / 8;
				for (int i = 0; i < nrOfBoxes; i++) {
					int x1 = corners.at(8 * i);
					int y1 = corners.at(8 * i + 1);
					int x2 = corners.at(8 * i + 2);
					int y2 = corners.at(8 * i + 3);
					int x3 = corners.at(8 * i + 4);
					int y3 = corners.at(8 * i + 5);
					int x4 = corners.at(8 * i + 6);
					int y4 = corners.at(8 * i + 7);


					viral_core::vector direction1 = directionsGrid.at(y1).at(x1);
					viral_core::vector direction2 = directionsGrid.at(y2).at(x2);
					viral_core::vector direction3 = directionsGrid.at(y3).at(x3);
					viral_core::vector direction4 = directionsGrid.at(y4).at(x4);

					viral_core::vector near_top_left = scaledCamPosition + (direction2 * 50);
					viral_core::vector near_top_right = scaledCamPosition + (direction1 * 50);
					viral_core::vector near_bot_left = scaledCamPosition + (direction3 * 50);
					viral_core::vector near_bot_right = scaledCamPosition + (direction4 * 50);
					viral_core::vector far_top_left = scaledCamPosition + (direction2 * 500);
					viral_core::vector far_top_right = scaledCamPosition + (direction1 * 500);
					viral_core::vector far_bot_left = scaledCamPosition + (direction3 * 500);
					viral_core::vector far_bot_right = scaledCamPosition + (direction4 * 500);

					addBoxWithCorners(geometry_mesh_frustums, near_top_left, near_top_right, near_bot_left, near_bot_right,
						far_top_left, far_top_right, far_bot_left, far_bot_right);


				}


			}
			

			


		}	//end of for each camera



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


		viral_core::auto_pointer<viral_core::model> geometry_cameraDrawings2D
			(new viral_core::model());
		geometry_cameraDrawings2D->insert_group("model_group_cameraDrawings2D", geometry_mesh_cameraDrawings2D);
		geometry_cameraDrawings2D->rebuild_boundings();
		geometry_cameraDrawings2D->validate();


		viral_core::auto_pointer<viral_core::model> geometry_frustums
			(new viral_core::model());
		geometry_frustums->insert_group("model_group_frustums", geometry_mesh_frustums);
		geometry_frustums->rebuild_boundings();
		geometry_frustums->validate();


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

		viral_core::render_model_data model_data_cameraDrawings2D;
		model_data_cameraDrawings2D.geometry = geometry_cameraDrawings2D;

		viral_core::render_model_data model_data_frustums;
		model_data_frustums.geometry = geometry_frustums;

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
		material_transparent_data.ambient_color = viral_core::color(0.05f, 0.6f, 0, 0.2f);
		material_transparent_data.diffuse_color = viral_core::color(0.05f, 0.6f, 0, 0.2f);
		material_transparent_data.specular_color = viral_core::color(0, 0, 0, 0.2f);
		material_transparent_data.blend = viral_core::render_material_data::blend_alpha;
		material_transparent_data.cull = viral_core::render_material_data::cull_none;
		material_transparent_data.shader = shader_id;

		viral_core::render_material_data material_transparent2_data;
		material_transparent2_data.ambient_color = viral_core::color(0.8f, 0.f, 0.05f, 0.3f);
		material_transparent2_data.diffuse_color = viral_core::color(0.8f, 0.f, 0.05f, 0.3f);
		material_transparent2_data.specular_color = viral_core::color(0, 0, 0, 0.2f);
		material_transparent2_data.blend = viral_core::render_material_data::blend_alpha;
		material_transparent2_data.cull = viral_core::render_material_data::cull_none;
		material_transparent2_data.shader = shader_id;


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


		//add the cameraDrawings2D model to the render_puppet_data
		viral_core::render_puppet_data puppet_data_cameraDrawings2D;
		puppet_data_cameraDrawings2D.position = viral_core::vector(0, 0, 0);
		puppet_data_cameraDrawings2D.model = model_cameraDrawings2D_id;
		puppet_data_cameraDrawings2D.materials.insert("model_group_cameraDrawings2D", material_unlit_id);


		//add the frustums model to the render_puppet_data
		viral_core::render_puppet_data puppet_data_frustums;
		puppet_data_frustums.position = viral_core::vector(0, 0, 0);
		puppet_data_frustums.model = model_frustums_id;
		puppet_data_frustums.materials.insert("model_group_frustums", material_transparent2_id);


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
		scene_data.objects.insert(puppet_cameraDrawings2D_id);
		scene_data.objects.insert(puppet_frustums_id);
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
		q->commit(model_data_cameraDrawings2D, model_cameraDrawings2D_id);
		q->commit(model_data_frustums, model_frustums_id);
		for (int i = 0; i < nrOfPuppets; i++)
			q->commit(model_data_cubes_container[i], model_cubes_ids[i]);

		q->commit(shader_data, shader_id);
		q->commit(material_data, material_id);
		q->commit(material_data_wireframe, material_wireframe_id);
		q->commit(material_data_unlit, material_unlit_id);
		q->commit(material_transparent_data, material_transparent_id);
		q->commit(material_transparent2_data, material_transparent2_id);
		
		// commit puppets
		q->commit(puppet_data_coordinateAxes, puppet_coordinateAxes_id);
		q->commit(puppet_data_boundingBox, puppet_boundingBox_id);
		q->commit(puppet_data_cameraPlanes, puppet_cameraPlanes_id);
		q->commit(puppet_data_cameraRays, puppet_cameraRays_id);
		q->commit(puppet_data_cameraDrawings2D, puppet_cameraDrawings2D_id);
		q->commit(puppet_data_frustums, puppet_frustums_id);
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