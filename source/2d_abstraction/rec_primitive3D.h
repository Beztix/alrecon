#ifndef REC_PRIMITIVE3D_H
#define REC_PRIMITIVE3D_H




#include <viral_core/geo_vector.hpp>



namespace rec {



	// a struct representing a single frustum
	struct frustum {

		viral_core::vector points[8];				//the 8 corners of the frustum
		viral_core::vector planeNormals[6];		//the normals of the 6 planes, always pointing inside the frustum
		viral_core::vector planeNormalBases[6];	//the base corners of the 6 normals

		enum vertex_type
		{
			near_xneg_ypos, near_xpos_ypos, near_xpos_yneg, near_xneg_yneg,
			far_xneg_ypos, far_xpos_ypos, far_xpos_yneg, far_xneg_yneg
		};

		enum plane_type
		{
			x_neg, x_pos, y_neg, y_pos, z_near, z_far
		};


		//empty constructor
		frustum() {

		}


		//constructor receiving the 8 corner points, automatically building the plane information
		frustum
			(const viral_core::vector &n_xn_yp, const viral_core::vector &n_xp_yp,
			const viral_core::vector &n_xp_yn, const viral_core::vector &n_xn_yn,
			const viral_core::vector &f_xn_yp, const viral_core::vector &f_xp_yp,
			const viral_core::vector &f_xp_yn, const viral_core::vector &f_xn_yn)
		{
			points[near_xneg_ypos] = n_xn_yp;
			points[near_xpos_ypos] = n_xp_yp;
			points[near_xpos_yneg] = n_xp_yn;
			points[near_xneg_yneg] = n_xn_yn;

			points[far_xneg_ypos] = f_xn_yp;
			points[far_xpos_ypos] = f_xp_yp;
			points[far_xpos_yneg] = f_xp_yn;
			points[far_xneg_yneg] = f_xn_yn;	

			build_planes();
		}


		//building the plane information
		void build_planes() {
			viral_core::vector vectorOne;
			viral_core::vector vectorTwo;
			viral_core::vector normal;
			viral_core::vector normalBase;

			//x_pos
			vectorOne = points[frustum::near_xpos_yneg] - points[frustum::near_xpos_ypos];
			vectorTwo = points[frustum::far_xpos_ypos] - points[frustum::near_xpos_ypos];
			planeNormals[frustum::x_pos] = vectorOne.cross(vectorTwo).normalized();
			planeNormalBases[frustum::x_pos] = points[frustum::near_xpos_ypos];

			//y_neg
			vectorOne = points[frustum::near_xneg_yneg] - points[frustum::near_xpos_yneg];
			vectorTwo = points[frustum::far_xpos_yneg] - points[frustum::near_xpos_yneg];
			planeNormals[frustum::y_neg] = vectorOne.cross(vectorTwo).normalized();
			planeNormalBases[frustum::y_neg] = points[frustum::near_xpos_yneg];

			//x_neg
			vectorOne = points[frustum::near_xneg_ypos] - points[frustum::near_xneg_yneg];
			vectorTwo = points[frustum::far_xneg_yneg] - points[frustum::near_xneg_yneg];
			planeNormals[frustum::x_neg] = vectorOne.cross(vectorTwo).normalized();
			planeNormalBases[frustum::x_neg] = points[frustum::near_xneg_yneg];

			//y_pos
			vectorOne = points[frustum::near_xpos_ypos] - points[frustum::near_xneg_ypos];
			vectorTwo = points[frustum::far_xneg_ypos] - points[frustum::near_xneg_ypos];
			planeNormals[frustum::y_pos] = vectorOne.cross(vectorTwo).normalized();
			planeNormalBases[frustum::y_pos] = points[frustum::near_xneg_ypos];

			//z_near
			vectorOne = points[frustum::near_xneg_ypos] - points[frustum::near_xpos_ypos];
			vectorTwo = points[frustum::near_xpos_yneg] - points[frustum::near_xpos_ypos];
			planeNormals[frustum::z_near] = vectorOne.cross(vectorTwo).normalized();
			planeNormalBases[frustum::z_near] = points[frustum::near_xpos_ypos];

			//z_far
			vectorOne = points[frustum::far_xneg_yneg] - points[frustum::far_xpos_yneg];
			vectorTwo = points[frustum::far_xpos_ypos] - points[frustum::far_xpos_yneg];
			planeNormals[frustum::z_far] = vectorOne.cross(vectorTwo).normalized();
			planeNormalBases[frustum::z_far] = points[frustum::far_xpos_yneg];

		}

	};
	









	// a struct representing a pyramid
	struct pyramid {

		viral_core::vector corners[5];			//the 4 corners of the base of the pyramid and the apex
		viral_core::vector planeNormals[4];		//the normals of the 4 planes corresponding the the sides of the pyramid, normals pointing inside
												//base corners of the normals are the corners at the same index



		enum corner_type		//as seen from the apex
		{
			bot_left, bot_right, top_right, top_left, apex		

		};

		enum plane_type			//as seen from the apex
		{
			bot, right, top, left
		};


		//empty constructor
		pyramid() {

		}


		//constructor receiving the apex and the 4 corners of the pyramid base, automatically building the plane information
		pyramid(viral_core::vector bot_left_corner, viral_core::vector bot_right_corner, 
			viral_core::vector top_right_corner, viral_core::vector top_left_corner, viral_core::vector apex_corner) {
			corners[bot_left] = bot_left_corner;
			corners[bot_right] = bot_right_corner;
			corners[top_right] = top_right_corner;
			corners[top_left] = top_left_corner;
			corners[apex] = apex_corner;

			buildPlanes();
		}


		//building the plane information
		void buildPlanes() {
			viral_core::vector vectorOne;
			viral_core::vector vectorTwo;
			viral_core::vector normal;
			viral_core::vector normalBase;

			//bot
			vectorOne = corners[apex] - corners[bot_left];
			vectorTwo = corners[bot_right] - corners[bot_left];
			planeNormals[bot] = vectorOne.cross(vectorTwo).normalized();

			//right
			vectorOne = corners[apex] - corners[bot_right];
			vectorTwo = corners[top_right] - corners[bot_right];
			planeNormals[right] = vectorOne.cross(vectorTwo).normalized();

			//top
			vectorOne = corners[apex] - corners[top_right];
			vectorTwo = corners[top_left] - corners[top_right];
			planeNormals[top] = vectorOne.cross(vectorTwo).normalized();

			//left
			vectorOne = corners[apex] - corners[top_left];
			vectorTwo = corners[bot_left] - corners[top_left];
			planeNormals[left] = vectorOne.cross(vectorTwo).normalized();


		}




	};





	struct aabb {

		viral_core::vector corners[8];		//the 8 corners of the aabb
		float extremalValues[6];


		enum vertex_type
		{
			near_xneg_ypos, near_xpos_ypos, near_xpos_yneg, near_xneg_yneg,
			far_xneg_ypos, far_xpos_ypos, far_xpos_yneg, far_xneg_yneg
		};

		enum extremal_type
		{
			minX, maxX, minY, maxY, minZ, maxZ
		};


		//empty constructor
		aabb() {

		}


		//constructor receiving the 8 corner points, automatically building the extremal values information
		aabb (const viral_core::vector &n_xn_yp, const viral_core::vector &n_xp_yp,
			const viral_core::vector &n_xp_yn, const viral_core::vector &n_xn_yn,
			const viral_core::vector &f_xn_yp, const viral_core::vector &f_xp_yp,
			const viral_core::vector &f_xp_yn, const viral_core::vector &f_xn_yn)
		{
			corners[near_xneg_ypos] = n_xn_yp;
			corners[near_xpos_ypos] = n_xp_yp;
			corners[near_xpos_yneg] = n_xp_yn;
			corners[near_xneg_yneg] = n_xn_yn;
			corners[far_xneg_ypos] = f_xn_yp;
			corners[far_xpos_ypos] = f_xp_yp;
			corners[far_xpos_yneg] = f_xp_yn;
			corners[far_xneg_yneg] = f_xn_yn;

			extremalValues[this->minX] = n_xn_yp.x;
			extremalValues[this->maxX] = n_xp_yp.x;
			extremalValues[this->minY] = n_xn_yn.y;
			extremalValues[this->maxY] = n_xn_yp.y;
			extremalValues[this->minZ] = n_xn_yn.z;
			extremalValues[this->maxZ] = f_xn_yn.z;

		}


		//constructor receiving the extremal value information, automatically building the 8 corner points
		aabb(const float minX, const float maxX, const float minY, const float maxY, const float minZ, const float maxZ) {
			extremalValues[this->minX] = minX;
			extremalValues[this->maxX] = maxX;
			extremalValues[this->minY] = minY;
			extremalValues[this->maxY] = maxY;
			extremalValues[this->minZ] = minZ;
			extremalValues[this->maxZ] = maxZ;

			corners[near_xneg_ypos] = viral_core::vector(minZ, minX, maxY);
			corners[near_xpos_ypos] = viral_core::vector(minZ, maxX, maxY);
			corners[near_xpos_yneg] = viral_core::vector(minZ, maxX, minY);
			corners[near_xneg_yneg] = viral_core::vector(minZ, minX, minY);
			corners[far_xneg_ypos] = viral_core::vector(maxZ, minX, maxY);
			corners[far_xpos_ypos] = viral_core::vector(maxZ, maxX, maxY);
			corners[far_xpos_yneg] = viral_core::vector(maxZ, maxX, minY);
			corners[far_xneg_yneg] = viral_core::vector(maxZ, minX, minY);


		}



	};







}








#endif /* REC_PRIMITIVE3D_H*/
