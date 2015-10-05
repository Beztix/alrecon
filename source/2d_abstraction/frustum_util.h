#ifndef FRUSTUM_UTIL_H
#define FRUSTUM_UTIL_H




#include <opencv2/core/core.hpp>



namespace frustum {






	

	// a struct representing a single frustum

	struct frust {

		cv::Point3f points[8];

		enum vertex_type
		{
			near_xneg_ypos, near_xpos_ypos, near_xpos_yneg, near_xneg_yneg,
			far_xneg_ypos, far_xpos_ypos, far_xpos_yneg, far_xneg_yneg
		};


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
		}

	};
	


}








#endif /* SE_UTIL_H*/
