/**
*************************************************************************
*
* @file main.cpp
*
* Main class for executing the various algorithms of alrecon.
* Supports Execution of:
* - alpha shapes with CGAL, a non-parallel implementation of the alpha shape algorithm in the computational geometry algorithms library
* - alpha shapes with GPUAlpha, an implementation of the alpha shape algorithm on the GPU
* - superellipses with ceres, an implementation of superellipse fitting using the non-linear least squares solver ceres
* - superellipses rosin, an implementation of superellipse fitting using a simple fit-algorithm developed by Paul Rosin
*
************************************************************************/

#define NOGDI

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <chrono>
#include <conio.h>

#include "image_input.h"
#include "image_output.h"
#include "util.h"
#include "se_util.h"
#include "tree.hh"
#include "tree_util.hh"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


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


// CGAL stuff
#include "as_cgal.h"


// GPUAlpha stuff
#include "as_gpu_input.h"


// Superellipses with ceres
#include <ceres/ceres.h>


// Superellipses with rosin
#include "se_rendering.h"
#include "se_rosin_core.h"
#include "se_rosin_recursive.h"
#include "se_rosin_tree.h"
#include "se_split.h"
#include "convexHull.h"
#include "convexHullForC.h"


// Reconstruction stuff
#include "rec_reconstruction_trivial.h"
#include "rec_reconstruction_frustum.h"
#include "rec_rendering.h"
#include "rec_sampling.h"
#include "rec_frustum_intersection.h"
#include "rec_frustum.h"
#include "rec_file_camera.h"
#include "rec_sensor.h"


#include <Windows.h>


using namespace std;






int main() {



	

	Sleep(2000);



	bool USE_CGAL =					false;
	bool USE_GPUALPHA =				false;
	bool USE_SUPERELLIPSES_ROSIN =	false;
	bool USE_SUPERELLIPSES_CERES =	false;
	bool TEST_SECTION =				true;

	

	//############################################
	//##########          CGAL          ##########       
	//############################################

	if (USE_CGAL) {

		//++++++++++++++++++++++++++++
		string inputName =	"image";
		int reduction =		5;
		//++++++++++++++++++++++++++++

		string inputImage = "images/" + inputName + ".png";
		string reducedImgName = "as_cgal_" + inputName + "_reducedImage.png";
		string outputImage = "as_cgal_" + inputName + "_alphaImage.png";

		useCgal(inputImage, reducedImgName, outputImage, reduction);
	}







	//############################################
	//##########        GPUAlpha        ##########       
	//############################################

	if (USE_GPUALPHA) {

		//++++++++++++++++++++++++++++
		bool PREPARE = true;

		string inputName =	"1920x1080_high";
		int reduction =		5; 
		//++++++++++++++++++++++++++++


		string inputImage = "images/" + inputName + ".png";
		string reducedImgName = "as_gpu_" + inputName + "_reducedImage.png";
		string pixelFile = "as_gpu_" + inputName + "_pixels.txt";


		if (PREPARE) {
			cout << "Preparing input for GPUAlpha" << endl;
			int err = prepareInputForAS(inputImage, reducedImgName, pixelFile, reduction);
			cout << "Prepared input from file " << inputImage << " to file " << pixelFile << endl;
		}

	}







	//#######################################################
	//##########        Superellipses Rosin        ##########       
	//#######################################################

	if (USE_SUPERELLIPSES_ROSIN) {

		for (int w = 1; w <= 7; w++) {


			//++++++++++++++++++++++++++++
			bool RECURSIVE = false;
			bool TREE = true;
			bool debug = false;


			string inputName = "occMask_" + std::to_string(w);
			int quality = 200;
			vector<int> qualityValues = { 50000, 10000, 1000, 200, 100 };
			int iterations = 1;
			//++++++++++++++++++++++++++++

			string inputImage = "images/" + inputName + ".png";

			int width;
			int height;

			LARGE_INTEGER frequency;        // ticks per second
			LARGE_INTEGER t1, t2, t3, t4, t5, t6;	//ticks
			QueryPerformanceFrequency(&frequency);
			double totalComputeDuration = 0;

			//load the contours from the image
			QueryPerformanceCounter(&t1);
			vector<vector<cv::Point>> contours = image_input::getContoursFromImage(inputImage, width, height);

			QueryPerformanceCounter(&t2);
			double prepareDuration = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
			cout << "prepareDuration:     " << prepareDuration << "ms" << endl;



			if (RECURSIVE) {
				string ellipseImage = "se_rosin_rec_" + inputName + "_ellipseImage_" + to_string(quality) + ".png";
				string combinedImage = "se_rosin_rec_" + inputName + "_combined" + to_string(quality) + ".png";

				vector<se::superellipse> totalEllipsesVector;

				//do the whole computation multiple times (performance averageing)
				for (int i = 0; i < iterations; i++) {
					totalEllipsesVector.clear();

					//use recursive rosin fitting to fit superellipses to the contours

					QueryPerformanceCounter(&t3);
					totalEllipsesVector = startRosinRecursive(contours, width, height, quality);

					QueryPerformanceCounter(&t4);
					double computeDuration = (t4.QuadPart - t3.QuadPart) * 1000.0 / frequency.QuadPart;
					cout << "computeDuration:     " << computeDuration << "ms" << endl;

					totalComputeDuration = totalComputeDuration + computeDuration;
				}

				cout << endl;
				cout << "totalComputeDuration:         " << totalComputeDuration << "ms" << endl;
				cout << "medium compute Duration:      " << totalComputeDuration / iterations << "ms" << endl;

				cout << endl;
				cout << "quality used:                 " << quality << endl;
				cout << "number of superellipses:      " << totalEllipsesVector.size() << endl;

				//render the fitted superellipses to an image file
				QueryPerformanceCounter(&t5);
				int err = processSuperellipsesFromVector(totalEllipsesVector, ellipseImage, width, height);

				//combine the original and the ellipse image
				image_output::combineOriginalAndEllipseImage(inputImage, ellipseImage, combinedImage);

				QueryPerformanceCounter(&t6);
				double renderDuration = (t6.QuadPart - t5.QuadPart) * 1000.0 / frequency.QuadPart;
				cout << "renderDuration:               " << renderDuration << "ms" << endl;
			}



			if (TREE) {
				string ellipseImagePart = "se_rosin_tree_" + inputName + "_ellipseImage_";
				string combinedImagePart = "se_rosin_tree_" + inputName + "_combined_";

				tree<se::contourAndSe> seTree;

				//do the whole computation multiple times (performance averaging)
				for (int i = 0; i < iterations; i++) {
					seTree.clear();

					QueryPerformanceCounter(&t3);

					//add root node to the tree
					tree<se::contourAndSe>::iterator top = seTree.begin();
					se::contourAndSe rootEllipse = se::contourAndSe();
					seTree.insert(top, rootEllipse);

					if (debug) {
						cout << endl;
						cout << "initial tree" << endl;
						kptree::print_tree_bracketed(seTree);
						cout << endl;
					}

					startRosinTree(seTree, contours, width, height, qualityValues);

					if (debug) {
						cout << endl;
						cout << "final tree" << endl;
						kptree::print_tree_bracketed(seTree);
						cout << endl;
					}

					QueryPerformanceCounter(&t4);
					double computeDuration = (t4.QuadPart - t3.QuadPart) * 1000.0 / frequency.QuadPart;
					cout << "computeDuration:     " << computeDuration << "ms" << endl;

					totalComputeDuration = totalComputeDuration + computeDuration;
				}

				int depth = seTree.max_depth();

				cout << endl;
				cout << "totalComputeDuration:         " << totalComputeDuration << "ms" << endl;
				cout << "medium compute Duration:      " << totalComputeDuration / iterations << "ms" << endl;

				cout << endl;
				cout << "quality used:                 " << quality << endl;
				cout << "depth of tree:                " << depth << endl;

				//render the fitted superellipses to image files
				QueryPerformanceCounter(&t5);

				for (int k = 1; k <= depth; k++) {
					vector<se::superellipse> ellipses = se::getEllipsesOfGivenDepth(seTree, k);
					processSuperellipsesFromVector(ellipses, ellipseImagePart + to_string(k) + ".png", width, height);

					processSuperellipsesToBoundingBoxFromVector(ellipses, inputName + "_lvl" + to_string(k), width, height, 300);

					//combine the original and the ellipse image
					image_output::combineOriginalAndEllipseImage(inputImage, ellipseImagePart + to_string(k) + ".png", combinedImagePart + to_string(k) + ".png");
				}



				QueryPerformanceCounter(&t6);
				double renderDuration = (t6.QuadPart - t5.QuadPart) * 1000.0 / frequency.QuadPart;
				cout << "renderDuration:               " << renderDuration << "ms" << endl;

			}
		}
	}







	//#######################################################
	//##########        Superellipses Ceres        ##########       
	//#######################################################

	if (USE_SUPERELLIPSES_CERES) {

		//++++++++++++++++++++++++++++
		bool PREPARE =			true;
		bool COMPUTE =			true;
		bool RENDER =			true;

		string inputName = "ellipsetest8";
		int reduction = 100;
		//++++++++++++++++++++++++++++

		string inputImage = "images/" + inputName + ".png";
		string reducedImgName = "se_ceres_" + inputName + "_reducedImage.png";
		string pixelFile = "se_ceres_" + inputName + "_pixels.txt";
		string ellipseTextFile = "se_ceres_" + inputName + "_textellipses";
		string ellipseImage = "se_ceres_" + inputName + "_ellipseImage.png";


		if (PREPARE) {
			cout << "Preparing input for superellipses - Ceres" << endl;
			//int err = prepareInputForCeres(inputImage, reducedImgName, pixelFile, reduction);
			cout << "Prepared input from file " << inputImage << " to file " << pixelFile << endl;
		}


		if (COMPUTE) {
			//double x, y, theta;
			//useCeresRecursive("allPixels", pixelFile, ellipseTextFile, "", "globalEllipses");

		}


		if (RENDER) {
			cout << "Rendering superellipses" << endl;
			//int err = processSuperellipsesFromTextfileCeres("globalEllipses", "globalEllipses.png");
			cout << "Rendered ellipses from file " << ellipseTextFile << " to Image " << ellipseImage << endl;

		}




	}







	





	//######################################################
	//##########           Test Section           ##########       
	//######################################################

	if (TEST_SECTION) {

		/*
		
		rec::frust firstFrust(cv::Point3f(0.0, 0.0, 0.0), cv::Point3f(2.0, 0.0, 0.0), cv::Point3f(2.0, -2.0, 0.0), cv::Point3f(0.0, -2.0, 0.0), 
								  cv::Point3f(0.0, 0.0, 4.0), cv::Point3f(2.0, 0.0, 4.0), cv::Point3f(2.0, -2.0, 4.0), cv::Point3f(0.0, -2.0, 4.0));

		rec::frust secondFrust(cv::Point3f(-1.0, 0.0, -2.0), cv::Point3f(1.0, 0.0, -2.0), cv::Point3f(1.0, -2.0, -2.0), cv::Point3f(-1.0, -2.0, -2.0),
							 	   cv::Point3f(-1.0, 0.0, 2.0), cv::Point3f(1.0, 0.0, 2.0), cv::Point3f(1.0, -2.0, 2.0), cv::Point3f(-1.0, -2.0, 2.0));


		bool intersect = rec::doFrustumsIntesect(firstFrust, secondFrust);

		cout << intersect << endl;


		

		

		file_camera cam(0);

		sensor sens(0, cam.image_size, sensor::projection_pinhole_distort);

		sens.set_pinhole_distort(cam.world_to_device_pinhole_distort_, cam.pinhole_distort_center_, cam.pinhole_distort_focus_, cam.distort_r1_, cam.distort_r2_, cam.distort_t1_, cam.distort_t2_);


		
		std::vector<viral_core::vector> projectedVectors;
		std::vector<int> pixels;

		for (int i = 0; i < (2200+2000)/20; i++) {
			int x = -2000 + 20 * i;
			viral_core::vector vec((float)x, 0, 0);

			viral_core::vector projVec = sens.project(vec);
			projectedVectors.emplace_back(projVec);
			pixels.emplace_back((int)projVec.x);
			pixels.emplace_back((int)projVec.y);
		}
		image_output::pixelVectorToImage(pixels, 1000, 1000, "projectionTestx.png");


		for (int i = 0; i < (2200 + 2200) / 20; i++) {
			int y = -2200 + 20 * i;
			viral_core::vector vec(0, (float)y, 0);

			viral_core::vector projVec = sens.project(vec);
			projectedVectors.emplace_back(projVec);
			pixels.emplace_back((int)projVec.x);
			pixels.emplace_back((int)projVec.y);
		}
		image_output::pixelVectorToImage(pixels, 1000, 1000, "projectionTesty.png");


		for (int i = 0; i < (880 + 1600) / 20; i++) {
			int z = -880 + 20 * i;
			viral_core::vector vec(0, 0, (float)z);

			viral_core::vector projVec = sens.project(vec);
			projectedVectors.emplace_back(projVec);
			pixels.emplace_back((int)projVec.x);
			pixels.emplace_back((int)projVec.y);
		}
		image_output::pixelVectorToImage(pixels, 1000, 1000, "projectionTestz.png");
		


		cout << "test" << endl;


		*/

		
		
		//rec::doAllSamplingCalculations();



		std::vector<viral_core::vector> occupiedWorldPositions = rec::reconstruct(20);

		rec::renderOccupiedPositions(occupiedWorldPositions, 10, 0.1f);
		
		

		


		


		/*

		


		///////// START RENDERING-DEMO

		viral_core::engine e("alrecon");
		viral_core::step_timer t(0.02f, 0.5f);


		// Resourcen
		viral_core::shared_pointer<viral_core::render_model_id> model_id
			(new viral_core::render_model_id("my_model"));
		viral_core::shared_pointer<viral_core::render_shader_id> shader_id
			(new viral_core::render_shader_id("my_shader"));
		viral_core::shared_pointer<viral_core::render_material_id> material_id
			(new viral_core::render_material_id("my_material"));



		// Scene
		viral_core::shared_pointer<viral_core::render_puppet_id> puppet_id
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


		// Put together geometry.
		viral_core::auto_pointer<viral_core::mesh> geometry_mesh
			(new viral_core::mesh());
		geometry_mesh->allocate_vertices(4);
		geometry_mesh->allocate_triangles(2);

		geometry_mesh->vertices()[0] = viral_core::vector(-1, -1, 0);
		geometry_mesh->vertices()[1] = viral_core::vector(1, -1, 0);
		geometry_mesh->vertices()[2] = viral_core::vector(1, 1, 0);
		geometry_mesh->vertices()[3] = viral_core::vector(-1, 1, 0);

		geometry_mesh->triangles()[0] = viral_core::mesh_triangle(0, 2, 1);
		geometry_mesh->triangles()[1] = viral_core::mesh_triangle(0, 3, 2);

		viral_core::auto_pointer<viral_core::mesh_stream_vector> geometry_mesh_normals
			(new viral_core::mesh_stream_vector());

		geometry_mesh_normals->resize(4);
		(*geometry_mesh_normals)[0] = viral_core::vector(0, 0, -1);
		(*geometry_mesh_normals)[1] = viral_core::vector(0, 0, -1);
		(*geometry_mesh_normals)[2] = viral_core::vector(0, 0, -1);
		(*geometry_mesh_normals)[3] = viral_core::vector(0, 0, -1);

		geometry_mesh->insert_vertex_stream
			(viral_core::mesh_stream_registry::normal_stream_name,
			geometry_mesh_normals);

		viral_core::auto_pointer<viral_core::model> geometry
			(new viral_core::model());
		geometry->insert_group("model_group", geometry_mesh);
		geometry->rebuild_boundings();
		geometry->validate();


		// Fill data structures for later render-side objects.
		viral_core::render_model_data model_data;
		model_data.geometry = geometry;

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

		viral_core::render_puppet_data puppet_data;
		puppet_data.position = viral_core::vector(0, 0, 16);
		puppet_data.model = model_id;
		puppet_data.materials.insert("model_group", material_id);

		viral_core::render_light_data light_data;
		light_data.emitter = viral_core::render_light_data::emitter_parallel;
		light_data.orientation =
			viral_core::rotation(viral_core::vector(1, 0, 0), -30) *
			viral_core::rotation(viral_core::vector(0, 1, 0), 30);

		viral_core::render_scene_data scene_data;
		scene_data.objects.insert(puppet_id);
		scene_data.objects.insert(light_id);

		viral_core::render_layer_data layer_data;
		layer_data.scene = scene_id;
		layer_data.background_color = viral_core::color(0.2f, 0, 0, 0);

		viral_core::render_canvas_data canvas_data;
		canvas_data.layers.insert(layer_id);
		canvas_data.copy_to_window = true;

		viral_core::render_process_data process_data;
		process_data.canvases.insert(canvas_id);
		process_data.show_results = true;


		// Commit data structures to render-side objects with appropriate ID.
		viral_core::auto_pointer<viral_core::render_command_queue> q
			(new viral_core::render_command_queue());
		q->commit(model_data, model_id);
		q->commit(shader_data, shader_id);
		q->commit(material_data, material_id);
		q->commit(puppet_data, puppet_id);
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
					return 0;

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



		*/

		
		





	
	}







	Sleep(2000);

}


