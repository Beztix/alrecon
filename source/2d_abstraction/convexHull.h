#ifndef CONVEXHULL_H
#define CONVEXHULL_H

#include <vector>
using namespace std;

typedef int coord_t;         // coordinate type

struct Point {
	coord_t x, y;

	bool operator <(const Point &p) const {
		return x < p.x || (x == p.x && y < p.y);
	}
};

vector<Point> convex_hull(vector<Point> P);






#endif /* CONVEXHULL_H*/
