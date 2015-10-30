#ifndef REC_OBJECT3D_H
#define REC_OBJECT3D_H




#include <viral_core/geo_3d.hpp>
#include <vector>


namespace rec {



	// a struct representing a 3D object in world space
	struct object3D {

		std::vector<viral_core::vector> planeNormals;		//the plane normals of the planes defining the object, always pointing inside the object
		std::vector<viral_core::vector> planeNormalBases;	//the base vectors of the plane normals

		

		//constructor receiving defining planeNormals and the corresponding base vectors
		object3D(std::vector<viral_core::vector> mplaneNormals, std::vector<viral_core::vector> mplaneNormalBases)
		{
			planeNormals = mplaneNormals;
			planeNormalBases = mplaneNormalBases;
		}


	};
	


}








#endif /* REC_OBJECT3D_H*/
