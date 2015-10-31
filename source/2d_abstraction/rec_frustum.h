#ifndef REC_FRUSTUM_H
#define REC_FRUSTUM_H




#include <viral_core/geo_3d.hpp>



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
			planeNormals[frustum::x_pos] = vectorOne.cross(vectorTwo);
			planeNormalBases[frustum::x_pos] = points[frustum::near_xpos_ypos];

			//y_neg
			vectorOne = points[frustum::near_xneg_yneg] - points[frustum::near_xpos_yneg];
			vectorTwo = points[frustum::far_xpos_yneg] - points[frustum::near_xpos_yneg];
			planeNormals[frustum::y_neg] = vectorOne.cross(vectorTwo);
			planeNormalBases[frustum::y_neg] = points[frustum::near_xpos_yneg];

			//x_neg
			vectorOne = points[frustum::near_xneg_ypos] - points[frustum::near_xneg_yneg];
			vectorTwo = points[frustum::far_xneg_yneg] - points[frustum::near_xneg_yneg];
			planeNormals[frustum::x_neg] = vectorOne.cross(vectorTwo);
			planeNormalBases[frustum::x_neg] = points[frustum::near_xneg_yneg];

			//y_pos
			vectorOne = points[frustum::near_xpos_ypos] - points[frustum::near_xneg_ypos];
			vectorTwo = points[frustum::far_xneg_ypos] - points[frustum::near_xneg_ypos];
			planeNormals[frustum::y_pos] = vectorOne.cross(vectorTwo);
			planeNormalBases[frustum::y_pos] = points[frustum::near_xneg_ypos];

			//z_near
			vectorOne = points[frustum::near_xneg_ypos] - points[frustum::near_xpos_ypos];
			vectorTwo = points[frustum::near_xpos_yneg] - points[frustum::near_xpos_ypos];
			planeNormals[frustum::z_near] = vectorOne.cross(vectorTwo);
			planeNormalBases[frustum::z_near] = points[frustum::near_xpos_ypos];

			//z_far
			vectorOne = points[frustum::far_xneg_yneg] - points[frustum::far_xpos_yneg];
			vectorTwo = points[frustum::far_xpos_ypos] - points[frustum::far_xpos_yneg];
			planeNormals[frustum::z_far] = vectorOne.cross(vectorTwo);
			planeNormalBases[frustum::z_far] = points[frustum::far_xpos_yneg];

		}

	};
	


}








#endif /* REC_FRUSTUM_H*/
