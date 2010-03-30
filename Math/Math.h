
#ifndef _DEREKMATH_H
#define _DEREKMATH_H

struct point {
	float x, y, z;
	point operator*(const float &rhs)
	{
		point result = *this;
		result.x *= rhs;
		result.y *= rhs;
		result.z *= rhs;
		return result;
	}
	float operator*(const point &rhs)
	{
		// Dot Product
		return (this->x * rhs.x) + (this->y * rhs.y) + (this->z * rhs.z);
	}
};

typedef point d_vector;

struct ray {
	point origin;
	point direction;
};

struct distloc {
    float distance;
    point intPoint;
};

#define dot(v1,v2) v1.x*v2.x+v1.y*v2.y+v1.z*v2.z

point FillPoint(float x, float y, float z);

d_vector Normal(point, point, point);
d_vector Normal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);

distloc Intersect(const ray &r, const point &v1, const point &v2, const point &v3, const point &v4);

void Normalize(d_vector &v);

d_vector ToVector(const point &p1, const point &p2);

float Distance(point p1, point p2);

float Magnitude(d_vector v);


#endif // _DEREKMATH_H

