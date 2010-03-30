
#include "Math/Math.h"
#include <cstdio>
#include <cmath>


float Magnitude(d_vector v)
{
	point origin;
	origin.x = origin.y = origin.z = 0;
	return Distance(origin, v);

}

d_vector Normal(point p1, point p2, point p3)
{
	return Normal(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z);



}

d_vector Normal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{


	d_vector toReturn;
	return toReturn;

}

float Distance(point p1, point p2)
{
    return sqrt(pow(p2.x-p1.x, 2) + pow(p2.y-p1.y,2) + pow(p2.z-p1.z,2));


}

point FillPoint(float x, float y, float z)
{
    point p;
    p.x = x;
    p.y = y;
    p.z = z;
    return p;
}


void Normalize(d_vector &v)
{
    float val = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    v.x  = v.x / val;
    v.y = v.y / val;
    v.z = v.z / val;
}

d_vector ToVector(const point &p1, const point &p2)
{
    d_vector toReturn;
    toReturn.x = p2.x - p1.x;
    toReturn.y = p2.y - p1.y;
    toReturn.z = p2.z - p1.z;

    return toReturn;

}



distloc Intersect(const ray &r, const point &v1, const point &v2, const point &v3, const point &v4)
{
    /*
     A = y1(z2-z3) + y2(z3-z1) + y3(z1-z2)
     B = z1(x2-x3) + z2(x3-x1) + z3(x1-x2)
     C = x1(y2-y3) + x2(y3-y1) + x3(y1-y2)
     D = -x1(y2*z3 - y3*z2) - x2(y3*z1 - y1*z3) - x3(y1*z2 - y2*z1)
     */

    //printf("Ray, o = (%lf, %lf, %lf), d = (%lf, %lf, %lf)\n", r.origin.x, r.origin.y, r.origin.z, r.direction.x, r.direction.y, r.direction.z);

    float A = v1.y * (v2.z - v3.z) + v2.y * (v3.z - v1.z) + v3.y * (v1.z - v2.z);
    float B = v1.z * (v2.x - v3.x) + v2.z * (v3.x - v1.x) + v3.z * (v1.x - v2.x);
    float C = v1.x * (v2.y - v3.y) + v2.x * (v3.y - v1.y) + v3.x  * (v1.y - v2.y);
    float D = -v1.x * (v2.y * v3.z - v3.y * v2.z) - v2.x * (v3.y * v1.z - v1.y * v3.z) - v3.x * (v1.y * v2.z - v2.y * v1.z);

    double t = -((double) A * (double) r.origin.x + (double) B
            * (double) r.origin.y + (double) C * (double) r.origin.z
            + (double) D) / ((double) A * (double) r.direction.x + (double) B
            * (double) r.direction.y + (double) C * (double) r.direction.z);

    distloc dl;
    if (t <= 0)
    {
        dl.intPoint.x = dl.intPoint.y = dl.intPoint.z = 0.0;
        dl.distance = 0.0;
        return dl;
    }

    point intPoint;
    intPoint.x = r.origin.x + r.direction.x * t;
    intPoint.y = r.origin.y + r.direction.y * t;
    intPoint.z = r.origin.z + r.direction.z * t;
    //intPoint.z *= -1.0;

    d_vector vv1, vv2, vv3, vv4, vv5;
    vv1 = ToVector(v1, v2);
    vv2 = ToVector(v1, v4);
    vv3 = ToVector(v3, v4);
    vv4 = ToVector(v1, intPoint);
    vv5 = ToVector(v3, intPoint);

    Normalize(vv1);
    Normalize(vv3);
    Normalize(vv4);
    Normalize(vv5);
    //printf("Testing %lf, %lf, %lf %lf\n", v1.z, v2.z, v1.x, v3.x);
    //printf("Testing %lf, %lf, %lf\n", intPoint.x, intPoint.y, intPoint.z);


    dl.intPoint = intPoint;

    if ((intPoint.x >= v1.x) && (intPoint.x <= v3.x) && (intPoint.y >= v1.y) && (intPoint.y <= v3.y) && (intPoint.z >= v1.z) && (intPoint.z <= v3.z))
    {
        //printf("Detected hit\n");
        dl.distance = t;

    }

    else
    {
        dl.distance = 0.0;
    }

    return dl;
	//printf("y's = %f, %f\n", v1.y, v2.y);

	//printf("A = %lf, B = %lf, C = %lf, D = %lf, t  = %.40f\n", A, B, C, D, t);
	//printf("(%f, %f, %f)\n", r.origin.x + r.direction.x*t,  r.origin.y + r.direction.y*t,  r.origin.z + r.direction.z*t);



}
