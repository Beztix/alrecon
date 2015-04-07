/*
*	=================
*	==  ORIGINAL:  ==
*	=================
*    estimate value of epsilon for superellipse
*    use intersection points of diagonals
*    take median of 4 such estimates
*
*    Paul Rosin
*    March 1998
*
*
*	================
*	==  CURRENT:  ==
*	================
*
*	Modified by Sebastian Lützow, March 2015
*	* pseudointersection is used instead of intersection to remove
*	  dependency on the order of the input
*	* in- and output is now via arrays instead of files on HDD (slow)
*	* external convex hull algorithm is used
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "convexHullForC.h"

#ifndef FALSE
# define FALSE 0
# define TRUE (!FALSE)
#endif

#define MAX_PIXELS 10000

#define MAX(a,b)     (((a) > (b)) ? (a) : (b))
#define MIN(a,b)     (((a) < (b)) ? (a) : (b))
#define ABS(x)       (((x)<0.0)? (-(x)): (x))

#define PI 3.1415927

int no_pixels;
int x[MAX_PIXELS], y[MAX_PIXELS];
double x2[MAX_PIXELS], y2[MAX_PIXELS];



/* use the fitting algorithm to fit one superellipse to the given contourPixels */

void fitEllipseRonin2(int* contourPixels, int no_contourPixels,
	double *xcOut, double *ycOut, double *thetaOut, double *aOut, double *bOut, double *epsilonOut)
{

	int i, j;
	double a, b, xc, yc, epsilon, t, theta;
	double xi1, yi1, xi2, yi2;
	double eps[4];
	no_pixels = no_contourPixels;

	//put data from pixels[] to x[] and y[]
	read_link_data(contourPixels);

	//calculate position, size and orientation of minimum bounding rectangle
	mbr(&xc, &yc, &a, &b, &theta);
	a /= 2.0;
	b /= 2.0;

	//set mbr to origin -> transform input accordingly
	normalise_data(xc, yc, theta);

	//pseudointersect the four diagonals of the mbr with the input to estimate epsilon
	pseudoIntersect(x2, y2, no_pixels, b, a, &xi1, &yi1, &xi2, &yi2);
	eps[0] = 2 * log(a / ABS(xi1)) / log(2.0);
	eps[1] = 2 * log(a / ABS(xi2)) / log(2.0);
	pseudoIntersect(x2, y2, no_pixels, -b, a, &xi1, &yi1, &xi2, &yi2);
	eps[2] = 2 * log(a / ABS(xi1)) / log(2.0);
	eps[3] = 2 * log(a / ABS(xi2)) / log(2.0);

	/*printf("eps[0]: %f\n", eps[0]);
	printf("eps[1]: %f\n", eps[1]);
	printf("eps[2]: %f\n", eps[2]);
	printf("eps[3]: %f\n", eps[3]);*/



	/* take median of estimates */
	for (i = 0; i < 3; i++) {
		for (j = i + 1; j < 4; j++) {
			if (eps[i] < eps[j]) {
				t = eps[i];
				eps[i] = eps[j];
				eps[j] = t;
			}
		}
	}

	/*printf("eps[0] new: %f\n", eps[0]);
	printf("eps[1] new: %f\n", eps[1]);
	printf("eps[2] new: %f\n", eps[2]);
	printf("eps[3] new: %f\n", eps[3]);*/

	//epsilon = (eps[1] + eps[2]) / 2.0;
	epsilon = (eps[1] + 2* eps[2] + 3*eps[3]) / 6.0;

	//set a threshold to prevent epsilon == 0 (may cause numerical problems) 
	if (epsilon < 0.01) {
		epsilon = 0.01;
	}

	*xcOut = xc;
	*ycOut = yc;
	*thetaOut = theta;
	*aOut = a;
	*bOut = b;
	*epsilonOut = epsilon;
}



/* shift and rotate to origin */
normalise_data(xc, yc, theta)
double xc, yc, theta;
{
	double cosine2 = cos(-theta);
	double sine2 = sin(-theta);
	double tx, ty;
	int i;

	for (i = 0; i < no_pixels; i++) {
		tx = x[i] - xc;
		ty = y[i] - yc;

		x2[i] = (tx * cosine2 - ty * sine2);
		y2[i] = (tx * sine2 + ty * cosine2);
	}
}



/* put the input in x[] and y[] */
read_link_data(int* pixels)
{
	for (int i = 0; i < no_pixels; i++) {
		x[i] = pixels[2 * i];
		y[i] = pixels[2 * i + 1];
	}
}



/* return approximated intersection points of ray "y = p/q x" and polygon */
/* approximation by finding the point closest to the ray and calculating the pedal point*/
pseudoIntersect(xdata, ydata, no_points, p, q, xi1, yi1, xi2, yi2)
double xdata[], ydata[];
int no_points;
double p, q;
double *xi1, *yi1, *xi2, *yi2;
{
	int i;
	double px, py, minpx_pos, minpy_pos, minpx_neg, minpy_neg;
	double a, b, f, dist, mindist_pos, mindist_neg, pedalx_pos, pedaly_pos, pedalx_neg, pedaly_neg;
	a = p;
	b = -q;
	double div = sqrt(a*a + b*b);
	mindist_pos = 100000;
	mindist_neg = 100000;
	//get closest point to ray on positive and on negative x-axis
	for (i = 0; i < no_points; i++) {
		px = xdata[i]; py = ydata[i];
		//get minimal distance from point to ray
		dist = fabs((a*px + b*py)) / div;
		//point on positive or negative x-axis?
		if (px >= 0) {
			if (dist < mindist_pos) {
				mindist_pos = dist;
				minpx_pos = px;
				minpy_pos = py;
			}
		}
		else {
			if (dist < mindist_neg) {
				mindist_neg = dist;
				minpx_neg = px;
				minpy_neg = py;
			}
		}
	}

	//calculate pedal point for positive x-axis
	f = a*minpy_pos - b*minpx_pos;
	pedaly_pos = (a*f) / (a*a + b*b);
	pedalx_pos = (a*pedaly_pos - f) / b;

	//calculate pedal point for negative x-axis
	f = a*minpy_neg - b*minpx_neg;
	pedaly_neg = (a*f) / (a*a + b*b);
	pedalx_neg = (a*pedaly_neg - f) / b;

	*xi1 = pedalx_pos;
	*yi1 = pedaly_pos;
	*xi2 = pedalx_neg;
	*yi2 = pedaly_neg;
}



/* calculate the minimum bounding rectangle */
mbr(xct, yct, a, b, rot)
double *xct, *yct, *a, *b, *rot;
{
	double p[MAX_PIXELS * 2];
	int n1, n2;
	int ch[MAX_PIXELS];
	int nch;
	static char directions[] = "norm";
	double epsilon = 0.001;
	int i, j, save_i;
	double dx, dy, theta, cosine, sine, x1, y1, x2, y2, x3, y3, x4, y4, xo, yo, xc, yc;
	double area, min_area;
	double min_x, min_y, max_x, max_y;

	for (i = 0; i < no_pixels; i++) {
		p[i * 2] = x[i];
		p[i * 2 + 1] = y[i];
	}
	n1 = no_pixels;

	//calculate convex hull
	external_convex_hull(p, n1, ch, &nch, directions, epsilon);

	/* go through all CH edges */
	save_i = 0;
	min_area = 9e9;
	for (i = 0; i < nch; i++) {
		n1 = ch[i];
		n2 = (ch[i + 1] % no_pixels);
		if (n1 == n2) continue;
		x1 = x[n1];
		y1 = y[n1];
		x2 = x[n2];
		y2 = y[n2];
		dx = x2 - x1; dy = y2 - y1;
		theta = atan2(dy, dx);
		cosine = cos(-theta);
		sine = sin(-theta);

		/* transform CH and test MBR */
		min_x = min_y = 9e9;
		max_x = max_y = -9e9;
		for (j = 0; j < nch; j++) {
			n1 = ch[j];
			x3 = x[n1] - x1;
			y3 = y[n1] - y1;
			x4 = cosine*x3 - sine*y3;
			y4 = sine*x3 + cosine*y3;

			//printf("%f,%f, %f,%f,\n", x3, y3, x4, y4);

			min_x = MIN(min_x, x4);
			min_y = MIN(min_y, y4);
			max_x = MAX(max_x, x4);
			max_y = MAX(max_y, y4);


		}
		area = (max_x - min_x) * (max_y - min_y);

		//printf("%f\n", area);

		if (area < min_area) {
			min_area = area;
			save_i = i;
		}
	}

	/* again with correct index */
	n1 = ch[save_i];
	n2 = (ch[save_i + 1] % no_pixels);
	x1 = x[n1];
	y1 = y[n1];
	x2 = x[n2];
	y2 = y[n2];
	dx = x2 - x1; dy = y2 - y1;
	theta = atan2(dy, dx);
	cosine = cos(-theta);
	sine = sin(-theta);
	min_x = min_y = 9e9;
	max_x = max_y = -9e9;
	for (i = 0; i < nch; i++) {
		n1 = ch[i];
		x3 = x[n1] - x1;
		y3 = y[n1] - y1;
		x4 = cosine*x3 - sine*y3;
		y4 = sine*x3 + cosine*y3;
		min_x = MIN(min_x, x4);
		min_y = MIN(min_y, y4);
		max_x = MAX(max_x, x4);
		max_y = MAX(max_y, y4);
	}
	cosine = cos(theta);
	sine = sin(theta);
	xo = x1; yo = y1;

	//printf("%f, %f, %f, %f\n", cosine, sine, xo, yo);

	/*** co-ords of MBR
	x1 = cosine*min_x - sine*min_y;
	y1 = sine*min_x + cosine*min_y;

	x2 = cosine*max_x - sine*min_y;
	y2 = sine*max_x + cosine*min_y;

	x3 = cosine*max_x - sine*max_y;
	y3 = sine*max_x + cosine*max_y;

	x4 = cosine*min_x - sine*max_y;
	y4 = sine*min_x + cosine*max_y;

	x1 += xo; y1 += yo;
	x2 += xo; y2 += yo;
	x3 += xo; y3 += yo;
	x4 += xo; y4 += yo;
	***/

	/* centre */
	xc = (min_x + max_x) / 2.0;
	yc = (min_y + max_y) / 2.0;

	*xct = cosine*xc - sine*yc + xo;
	*yct = sine*xc + cosine*yc + yo;

	*a = max_x - min_x;
	*b = max_y - min_y;

	if (*a > *b) {
		*rot = PI - theta;
	}
	else {
		*rot = theta + (PI / 2.0);
	}
	/*
	if (*rot < 0) *rot += 2*PI; if (*rot < 0) *rot += 2*PI;
	if (*rot > PI) *rot -= PI; if (*rot > PI) *rot -= PI;
	*/
	*rot = theta;
	if (*rot < 0) *rot += 2 * PI; if (*rot < 0) *rot += 2 * PI;
}



















//
//
//
//
//
///* ############################################################ */
//
///*
//convexhull.c
//
//Find the convex hull of a set of points.
//
//The algorithm used is as describe in
//
//Shamos, Michael,  "Problems
//in Computational Geometry"  May, 1975 (a set of photocopies --
//QA 447.S52 1983 in Carlson).
//
//It originally appeared in a more complicated form in
//
//Jarvis, R.A., "On the Identification of the Convex Hull of a
//Finite Set of Points in the Plane", Info. Proc. Letters 2(1973),
//pp. 18-21.
//
//The algorithm is of complexity k*n, where n is the number of points in the
//input and k the number of points in the convex hull.
//
//usage:
//convex_hull(p,n,ch,&nch,directions);
//where p is an n*2 array of doubles containing the set of points,
//n is the number of points,
//ch is an array of size n integers to hold the list of points in the
//convex hull, numbered 0 to nch-1;
//In nch the number of points in the convex hull is returned.
//directions is either "full" or "norm".  If directions="full" all the
//possible points on the convex hull are returned.  If directions="norm"
//a minimal set of points to describe the convex hull is returned.
//
//epsilon is the angle tolerance in radians.  When two angles are closer than
//PI radians they are considered equal.
//*/
//
///* ### CONTENTS OF INCLUDE FILE INSERTED HERE ### */
//
///* incl.h */
//
//#define boolean int
//#define true 1
//#define false 0
//
//#define ERROR -1
//#define OK 0
//
//#define EPSILON 1.e-6
//#define BIGNUM 1.e20
//
//#define sqr(x) ((x)*(x))
//
///* ### -------------------------------------- ### */
//
//static int debug = 0;
//
//convex_hull(p, n, ch, p_nch, directions, epsilon)
//double *p;
//int n, *p_nch, *ch;
//char *directions;
//double epsilon;
//{
//	double phi, max_phi, dist, max_cen_dist, min_dist, max_dist,
//		x, y, cen_x, cen_y, xp, yp,
//		xx, yy, m[2][2];
//	int i, i_keep, vertex, furthest, ch_vert;
//	boolean full;
//
//	if (!strcmp(directions, "full")) {
//		full = true;
//	}
//	else if (!strcmp(directions, "norm")) {
//		full = false;
//	}
//	else {
//		fprintf(stderr, "convex_hull: invalid argument \"%s\"\n", directions);
//		exit(ERROR);
//	}
//	centroid(p, n, &cen_x, &cen_y);
//	/* find the point furthest from the centroid */
//	max_cen_dist = 0.;
//	for (i = 0; i<n; i++) {
//		x = p[2 * i];
//		y = p[2 * i + 1];
//		dist = sqrt(sqr(x - cen_x) + sqr(y - cen_y));
//		if (dist>max_cen_dist) {
//			max_cen_dist = dist;
//			furthest = i;
//		}
//	}
//
//	/*
//	Determine rotation matrix so that coordinate system for determining
//	the orientations of line segments is wrt the line from the point
//	under consideration to the centroid.  Then all angles will be
//	< 90 degrees.  To maintain the strict inequality the furthest
//	point along the extreme ray will be used as the next point on
//	the convex hull.
//	*/
//
//	make_rotation_matrix((cen_x - p[furthest * 2]) / max_cen_dist,
//		(cen_y - p[furthest * 2 + 1]) / max_cen_dist, (double *)m);
//	ch_vert = 0;
//	vertex = furthest;
//	do {
//		ch[ch_vert] = vertex;
//		/* Find the ray with the maximum and minimum angle in the new
//		coordinate system */
//		if (debug) printf("vertex %d\n", vertex);
//		max_phi = -BIGNUM;
//		min_dist = BIGNUM;
//		max_dist = 0.;
//		for (i = 0; i<n; i++) {
//			/* calculate phi, the angle in the new coordinate system */
//			x = p[2 * i] - p[2 * vertex];
//			y = p[2 * i + 1] - p[2 * vertex + 1];
//			xp = m[0][0] * x + m[0][1] * y;
//			yp = m[1][0] * x + m[1][1] * y;
//			if (debug) {
//				printf("\ti %d x %f y %f xp %f yp %f\n", i, x, y, xp, yp);
//			}
//			if ((dist = sqrt(sqr(x) + sqr(y))) > EPSILON) {
//				phi = atan2(yp, xp);
//				if (debug) printf("\tphi %f\n", phi);
//				if (phi > max_phi - epsilon) {
//					if (full) {
//						/* use the closest point */
//						if (phi > max_phi + epsilon || dist < min_dist) {
//							min_dist = dist;
//							max_phi = phi;
//							i_keep = i;
//						}
//					}
//					else {
//						/* use the furthest point */
//						if (phi > max_phi + epsilon || dist > max_dist) {
//							max_dist = dist;
//							max_phi = phi;
//							i_keep = i;
//						}
//					}
//					if (debug) {
//						printf("\t\tmax_phi %f i_keep %d\n", max_phi, i_keep);
//					}
//				}
//			}
//		} /* for */
//		vertex = i_keep;
//		xx = cen_x - p[vertex * 2];
//		yy = cen_y - p[vertex * 2 + 1];
//		dist = sqrt(sqr(xx) + sqr(yy));
//		make_rotation_matrix(xx / dist, yy / dist, (double *)m);
//		ch_vert++;
//	} while (vertex != ch[0]);
//	*p_nch = ch_vert;
//}
//
//
//centroid(pts, n, p_cen_x, p_cen_y)
///*
//Determines the centroid of the set of n points in the n*2 array of
//doubles pts and returns its x-coordinate and y-coordinate in p_cen_x
//and p_cen_y respectively.
//*/
//double *pts, *p_cen_x, *p_cen_y;
//int n;
//{
//	double sumx, sumy;
//	int i;
//
//	sumx = sumy = 0;
//	for (i = 0; i<n; i++) {
//		sumx += pts[2 * i];
//		sumy += pts[2 * i + 1];
//	}
//	*p_cen_x = sumx / n;
//	*p_cen_y = sumy / n;
//}
//
//
//make_rotation_matrix(cos_theta, sin_theta, m)
//double cos_theta, sin_theta, *m;
///*
//Given a unit vector (vx,vy) system, finds the matrix m that
//corresponds to rotating the coordinate system so that its x-axis
//lies in the direction defined by (vx,vy).
//*/
//{
//	*m = cos_theta;
//	*(m + 1) = sin_theta;
//	*(m + 2) = -sin_theta;
//	*(m + 3) = cos_theta;
//}
