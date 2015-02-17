/*
 *    estimate value of epsilon for superellipse
 *    use area of polygon - assumes closed pixel list
 *    use look-up table to correct estimate
 *    use MBR to estimate a, b, centre, and orientation
 *
 *    Paul Rosin
 *    March 1998
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef FALSE
# define FALSE 0
# define TRUE (!FALSE)
#endif

#define MAX_PIXELS 10000

#define MAX(a,b)     (((a) > (b)) ? (a) : (b))
#define MIN(a,b)     (((a) < (b)) ? (a) : (b))

#define PI 3.1415927

int no_pixels;
int x[MAX_PIXELS],y[MAX_PIXELS];

double true[] = {
    0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7,
    0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35,
    1.4, 1.45, 1.5, 1.55, 1.6, 1.65, 1.7, 1.75, 1.8, 1.85, 1.9, 1.95, 2.0,
    2.05, 2.1, 2.15, 2.2, 2.25, 2.3, 2.35, 2.4, 2.45, 2.5, 2.55, 2.6, 2.65,
    2.7, 2.75, 2.8, 2.85, 2.9, 2.95, 3};

double estimate[] = {
    0.318584, 0.365502, 0.398717, 0.440398, 0.486608, 0.536431, 0.586335,
    0.633462, 0.685600, 0.736455, 0.795713, 0.844208, 0.898549, 0.955068,
    1.002863, 1.061981, 1.105906, 1.157625, 1.207333, 1.257242, 1.305059,
    1.351158, 1.396603, 1.442770, 1.487420, 1.530119, 1.577719, 1.617412,
    1.659471, 1.698296, 1.738293, 1.777394, 1.817147, 1.855153, 1.889113,
    1.926441, 1.963253, 1.997633, 2.031319, 2.061589, 2.094047, 2.124089,
    2.154160, 2.182415, 2.213236, 2.241130, 2.268343, 2.293863, 2.322353,
    2.347539, 2.374734, 2.396072, 2.420877, 2.443699, 2.465211, 2.487450,
    2.510880, 2.533847, 2.552825};

double correct();

int calculateEllipse(char* inputFile, char* outputFile)
{
    FILE *fp1,*fp2;
    char file_type[50];
    int j;
    int endoffile;
    double a,b,xc,yc,xc2,yc2,area,epsilon,t,theta;
    int flag;
    char *outfile = NULL;

    int constrain = FALSE; /* constrain fit to be oval */

    
    outfile = outputFile;


	if ((fp1 = fopen(inputFile, "r")) == NULL){
		printf("cant open %s\n", inputFile);
		exit(-1);
	}


    if((fp2=fopen(outputFile,"w")) == NULL){
        printf("cant open %s\n",outputFile);
        exit(-1);
    }
   
	
	fprintf(fp2,"super\nlist: 0\n");
    

    /* read magic word for format of file */
    fscanf(fp1,"%s\n",file_type);
    j = strcmp(file_type,"pixel");
    if(j != 0){
        printf("not link data file - aborting\n");
        exit(-1);
    }

    do {
        read_link_data(fp1,&endoffile);

        mbr(&xc,&yc,&a,&b,&theta);
        a /= 2.0;
        b /= 2.0;

        /*
        a = 200;
        b = 100;
        */

        flag = polyCentroid(x,y,no_pixels,&xc2,&yc2,&area);
        if (area < 0) area = -area;

        t = area / (4.0*a*b);
        epsilon = 1 -  t + sqrt(t*t-6*t+5);

        if (outfile == NULL) {
            printf("ctr %f %f\n",xc,yc);
            printf("a b %f %f\n",MAX(a,b),MIN(a,b));
            /*
            printf("theta %f\n",theta*1/PI);
            */
            printf("theta %f\n",theta);
            /*
            printf("epsilon %f\n",epsilon);
            */
            printf("epsilon %f\n",correct(epsilon));
        }
        else {
            if (constrain) epsilon = MIN(1,epsilon);
            
            fprintf(fp2,"superellipse: 0.0 %.0f %.0f %d %d %d %d %f %f %f %f %d\n",
                xc,yc,
                0,0,0,0,
                a,b,epsilon,theta,
                1);
        }

    } while (!endoffile);
    fclose(fp1);
    if (outfile != NULL) {
        fprintf(fp2,"endl:\nendf:\n");
        fclose(fp2);
    }
}

read_link_data(fp,endoffile)
FILE *fp;
int *endoffile;
{
    char dumstring[50];
    int j;

    fscanf(fp,"%s %d\n",dumstring,&j);
    j = -1;
    do{
       j++;
       fscanf(fp,"%d %d\n",&x[j],&y[j]);
    } while(x[j] != -1);
    *endoffile = (y[j] == -1);
    no_pixels = j;
}

/*
 * ANSI C code from the article
 * "Centroid of a Polygon"
 * by Gerard Bashein and Paul R. Detmer,
    (gb@locke.hs.washington.edu, pdetmer@u.washington.edu)
 * in "Graphics Gems IV", Academic Press, 1994
 */

/*********************************************************************
polyCentroid: Calculates the centroid (xCentroid, yCentroid) and area
of a polygon, given its vertices (x[0], y[0]) ... (x[n-1], y[n-1]). It
is assumed that the contour is closed, i.e., that the vertex following
(x[n-1], y[n-1]) is (x[0], y[0]).  The algebraic sign of the area is
positive for counterclockwise ordering of vertices in x-y plane;
otherwise negative.

Returned values:  0 for normal execution;  1 if the polygon is
degenerate (number of vertices < 3);  and 2 if area = 0 (and the
centroid is undefined).
**********************************************************************/
int polyCentroid(int x[], int y[], int n,
         double *xCentroid, double *yCentroid, double *area)
{
     register int i, j;
     double ai, atmp = 0, xtmp = 0, ytmp = 0;
     if (n < 3) return 1;
     for (i = n-1, j = 0; j < n; i = j, j++) {
         ai = x[i] * y[j] - x[j] * y[i];
         atmp += ai;
         xtmp += (x[j] + x[i]) * ai;
         ytmp += (y[j] + y[i]) * ai;
     }
     *area = atmp / 2;
     if (atmp != 0) {
         *xCentroid = xtmp / (3 * atmp);
         *yCentroid = ytmp / (3 * atmp);
         return 0;
     }
     return 2;
}

/* use linear interpolated table look-up to correct epsilon estimate */
double correct(v)
double v;
{
    int i=0;
    double delta_est,delta_true,delta_value,ratio,interp;

    do {
        i++;
        if (i >= 59)
            break;
        if (v < estimate[i])
            break;
    } while (TRUE);
    /***
    printf("interpolating between %f at %d and %f at %d\n",
           estimate[i-1],i-1,estimate[i],i);
    ***/
    if ((i >= 59) || (v < estimate[i-1])) {
        /*
        fprintf(stderr,"ERROR: cannot interpolate %f\n",v);
        exit(-1);
        */
        interp = v;
    }
    else {
        delta_est = estimate[i] - estimate[i-1];
        delta_true = true[i] - true[i-1];
        delta_value = v - estimate[i-1];
        ratio = delta_value / delta_est;
        /* interpolated corrected value */
        interp = true[i-1] + ratio * delta_true;
    }

    return(interp);
}

mbr(xct,yct,a,b,rot)
double *xct,*yct,*a,*b,*rot;
{
    double p[MAX_PIXELS*2];
    int n1,n2;
    int ch[MAX_PIXELS];
    int nch;
    static char directions[] = "norm";
    double epsilon = 0.001;
    int i,j,save_i;
    double dx,dy,theta,cosine,sine,x1,y1,x2,y2,x3,y3,x4,y4,xo,yo,xc,yc;
    double area,min_area;
    double min_x,min_y,max_x,max_y;

    for (i = 0; i < no_pixels; i++) {
        p[i*2] = x[i];
        p[i*2+1] = y[i];
    }
    n1 = no_pixels;

    convex_hull(p,n1,ch,&nch,directions,epsilon);

    /* go through all CH edges */
    save_i = 0;
    min_area = 9e9;
    for (i = 0; i < nch; i++) {
        n1 = ch[i];
        n2 = (ch[i+1] % no_pixels);
        if (n1 == n2) continue;
        x1 = x[n1];
        y1 = y[n1];
        x2 = x[n2];
        y2 = y[n2];
        dx = x2 - x1; dy = y2 - y1;
        theta = atan2(dy,dx);
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
            min_x = MIN(min_x,x4);
            min_y = MIN(min_y,y4);
            max_x = MAX(max_x,x4);
            max_y = MAX(max_y,y4);
        }
        area = (max_x - min_x) * (max_y - min_y);
        if (area < min_area) {
            min_area = area;
            save_i = i;
        }
    }

    /* again with correct index */
    n1 = ch[save_i];
    n2 = (ch[save_i+1] % no_pixels);
    x1 = x[n1];
    y1 = y[n1];
    x2 = x[n2];
    y2 = y[n2];
    dx = x2 - x1; dy = y2 - y1;
    theta = atan2(dy,dx);
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
        min_x = MIN(min_x,x4);
        min_y = MIN(min_y,y4);
        max_x = MAX(max_x,x4);
        max_y = MAX(max_y,y4);
    }
    cosine = cos(theta);
    sine = sin(theta);
    xo = x1; yo = y1;

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
        *rot = PI-theta;
    }
    else {
        *rot = theta+(PI/2.0);
    }
    /*
    if (*rot < 0) *rot += 2*PI; if (*rot < 0) *rot += 2*PI;
    if (*rot > PI) *rot -= PI; if (*rot > PI) *rot -= PI;
    */
    *rot = theta;
    if (*rot < 0) *rot += 2*PI; if (*rot < 0) *rot += 2*PI;
}

/* ############################################################ */

/*
convexhull.c

Find the convex hull of a set of points.

The algorithm used is as describe in

Shamos, Michael,  "Problems
in Computational Geometry"  May, 1975 (a set of photocopies --
QA 447.S52 1983 in Carlson).

It originally appeared in a more complicated form in

Jarvis, R.A., "On the Identification of the Convex Hull of a
Finite Set of Points in the Plane", Info. Proc. Letters 2(1973),
pp. 18-21.

The algorithm is of complexity k*n, where n is the number of points in the
input and k the number of points in the convex hull.

usage:
convex_hull(p,n,ch,&nch,directions);
where p is an n*2 array of doubles containing the set of points,
n is the number of points,
ch is an array of size n integers to hold the list of points in the
    convex hull, numbered 0 to nch-1;
In nch the number of points in the convex hull is returned.
directions is either "full" or "norm".  If directions="full" all the
possible points on the convex hull are returned.  If directions="norm"
a minimal set of points to describe the convex hull is returned.

epsilon is the angle tolerance in radians.  When two angles are closer than
PI radians they are considered equal.
*/

/* ### CONTENTS OF INCLUDE FILE INSERTED HERE ### */

/* incl.h */

#define boolean int
#define true 1
#define false 0

#define ERROR -1
#define OK 0

#define EPSILON 1.e-6
#define BIGNUM 1.e20

#define sqr(x) ((x)*(x))

/* ### -------------------------------------- ### */

static int debug=0;

convex_hull(p,n,ch,p_nch,directions,epsilon)
double *p;
int n, *p_nch, *ch;
char *directions;
double epsilon;
{
    double phi, max_phi, dist, max_cen_dist, min_dist, max_dist,
        x, y, cen_x, cen_y, xp, yp,
    xx, yy, m[2][2];
    int i, i_keep, vertex, furthest, ch_vert;
    boolean full;

    if (!strcmp(directions,"full")) {
    full = true;
    }
    else if (!strcmp(directions,"norm")) {
    full = false;
    }
    else {
    fprintf(stderr,"convex_hull: invalid argument \"%s\"\n",directions);
    exit(ERROR);
    }
    centroid(p,n,&cen_x,&cen_y);
    /* find the point furthest from the centroid */
    max_cen_dist = 0.;
    for (i=0; i<n; i++) {
    x = p[2*i];
    y = p[2*i+1];
    dist = sqrt(sqr(x-cen_x) + sqr(y-cen_y));
    if (dist>max_cen_dist) {
        max_cen_dist = dist;
        furthest = i;
    }
    }

    /*
    Determine rotation matrix so that coordinate system for determining
    the orientations of line segments is wrt the line from the point
    under consideration to the centroid.  Then all angles will be
    < 90 degrees.  To maintain the strict inequality the furthest
    point along the extreme ray will be used as the next point on
    the convex hull.
    */

    make_rotation_matrix((cen_x-p[furthest*2])/max_cen_dist,
             (cen_y-p[furthest*2+1])/max_cen_dist,(double *)m);
    ch_vert = 0;
    vertex = furthest;
    do {
    ch[ch_vert] = vertex;
    /* Find the ray with the maximum and minimum angle in the new
       coordinate system */
    if (debug) printf("vertex %d\n",vertex);
    max_phi = - BIGNUM;
    min_dist = BIGNUM;
    max_dist = 0.;
    for (i=0; i<n; i++) {
        /* calculate phi, the angle in the new coordinate system */
        x = p[2*i] - p[2*vertex];
        y = p[2*i+1] - p[2*vertex+1];
        xp = m[0][0]*x + m[0][1]*y;
        yp = m[1][0]*x + m[1][1]*y;
        if (debug) {
        printf("\ti %d x %f y %f xp %f yp %f\n", i,x,y,xp,yp);
        }
        if ((dist=sqrt(sqr(x)+sqr(y))) > EPSILON) {
        phi = atan2(yp,xp);
        if (debug) printf("\tphi %f\n", phi);
        if (phi > max_phi-epsilon) {
            if (full) {
            /* use the closest point */
            if (phi > max_phi + epsilon || dist < min_dist) {
                min_dist = dist;
                max_phi = phi;
                i_keep = i;
            }
            }
            else {
            /* use the furthest point */
            if (phi > max_phi + epsilon || dist > max_dist) {
                max_dist = dist;
                max_phi = phi;
                i_keep = i;
            }
            }
            if (debug) {
            printf("\t\tmax_phi %f i_keep %d\n", max_phi,i_keep);
            }
        }
        }
    } /* for */
        vertex = i_keep;
    xx = cen_x - p[vertex*2];
    yy = cen_y - p[vertex*2+1];
    dist = sqrt(sqr(xx) + sqr(yy));
    make_rotation_matrix(xx/dist,yy/dist,(double *)m);
    ch_vert++;
    } while (vertex != ch[0]);
    *p_nch = ch_vert;
}


centroid(pts,n,p_cen_x, p_cen_y)
/*
Determines the centroid of the set of n points in the n*2 array of
doubles pts and returns its x-coordinate and y-coordinate in p_cen_x
and p_cen_y respectively.
*/
double *pts, *p_cen_x, *p_cen_y;
int n;
{
    double sumx, sumy;
    int i;

    sumx = sumy = 0;
    for (i=0; i<n; i++) {
    sumx += pts[2*i];
    sumy += pts[2*i+1];
    }
    *p_cen_x = sumx / n;
    *p_cen_y = sumy / n;
}


make_rotation_matrix(cos_theta,sin_theta,m)
double cos_theta, sin_theta, *m;
/*
Given a unit vector (vx,vy) system, finds the matrix m that
corresponds to rotating the coordinate system so that its x-axis
lies in the direction defined by (vx,vy).
*/
{
    *m = cos_theta;
    *(m+1) = sin_theta;
    *(m+2) = - sin_theta;
    *(m+3) = cos_theta;
}
