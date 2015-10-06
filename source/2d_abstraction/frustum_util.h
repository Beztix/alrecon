#ifndef FRUSTUM_UTIL_H
#define FRUSTUM_UTIL_H




#include <opencv2/core/core.hpp>



namespace fr {



	// a struct representing a single frustum
	struct frust {

		cv::Point3f points[8];				//the 8 corners of the frustum
		cv::Point3f planeNormals[6];		//the normals of the 6 planes, always pointing inside the frustum
		cv::Point3f planeNormalBases[6];	//the base corners of the 6 normals

		enum vertex_type
		{
			near_xneg_ypos, near_xpos_ypos, near_xpos_yneg, near_xneg_yneg,
			far_xneg_ypos, far_xpos_ypos, far_xpos_yneg, far_xneg_yneg
		};

		enum plane_type
		{
			x_neg, x_pos, y_neg, y_pos, z_near, z_far
		};


		//constructor receiving the 8 corner points, automatically building the plane information
		frust
			(const cv::Point3f &n_xn_yp, const cv::Point3f &n_xp_yp,
			const cv::Point3f &n_xp_yn, const cv::Point3f &n_xn_yn,
			const cv::Point3f &f_xn_yp, const cv::Point3f &f_xp_yp,
			const cv::Point3f &f_xp_yn, const cv::Point3f &f_xn_yn)
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
			cv::Point3f vectorOne;
			cv::Point3f vectorTwo;
			cv::Point3f normal;
			cv::Point3f normalBase;

			//x_pos
			vectorOne = points[frust::near_xpos_yneg] - points[frust::near_xpos_ypos];
			vectorTwo = points[frust::far_xpos_ypos] - points[frust::near_xpos_ypos];
			planeNormals[frust::x_pos] = vectorOne.cross(vectorTwo);
			planeNormalBases[frust::x_pos] = points[frust::near_xpos_ypos];

			//y_neg
			vectorOne = points[frust::near_xneg_yneg] - points[frust::near_xpos_yneg];
			vectorTwo = points[frust::far_xpos_yneg] - points[frust::near_xpos_yneg];
			planeNormals[frust::y_neg] = vectorOne.cross(vectorTwo);
			planeNormalBases[frust::y_neg] = points[frust::near_xpos_yneg];

			//x_neg
			vectorOne = points[frust::near_xneg_ypos] - points[frust::near_xneg_yneg];
			vectorTwo = points[frust::far_xneg_yneg] - points[frust::near_xneg_yneg];
			planeNormals[frust::x_neg] = vectorOne.cross(vectorTwo);
			planeNormalBases[frust::x_neg] = points[frust::near_xneg_yneg];

			//y_pos
			vectorOne = points[frust::near_xpos_ypos] - points[frust::near_xneg_ypos];
			vectorTwo = points[frust::far_xneg_ypos] - points[frust::near_xneg_ypos];
			planeNormals[frust::y_pos] = vectorOne.cross(vectorTwo);
			planeNormalBases[frust::y_pos] = points[frust::near_xneg_ypos];

			//z_near
			vectorOne = points[frust::near_xneg_ypos] - points[frust::near_xpos_ypos];
			vectorTwo = points[frust::near_xpos_yneg] - points[frust::near_xpos_ypos];
			planeNormals[frust::z_near] = vectorOne.cross(vectorTwo);
			planeNormalBases[frust::z_near] = points[frust::near_xpos_ypos];

			//z_far
			vectorOne = points[frust::far_xneg_yneg] - points[frust::far_xpos_yneg];
			vectorTwo = points[frust::far_xpos_ypos] - points[frust::far_xpos_yneg];
			planeNormals[frust::z_far] = vectorOne.cross(vectorTwo);
			planeNormalBases[frust::z_far] = points[frust::far_xpos_yneg];

		}

	};
	


}








#endif /* SE_UTIL_H*/
