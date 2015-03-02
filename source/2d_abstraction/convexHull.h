#ifndef CONVEXHULL_H
#define CONVEXHULL_H

//#include <vector>
//using namespace std;
//
//typedef int coord_t;         // coordinate type
//
//struct Point {
//	coord_t x, y;
//
//	bool operator <(const Point &p) const {
//		return x < p.x || (x == p.x && y < p.y);
//	}
//};
//
//vector<Point> convex_hull(vector<Point> P);
//



#if defined(__cplusplus)
	extern "C"
#endif
int external_convex_hull(double *p, int n, int *ch, int *p_nch, char *directions, double epsilon);





#endif /* CONVEXHULL_H*/
