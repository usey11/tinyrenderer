#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <vector>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct Vec2 {
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
	t&             operator [](int i)            { return raw[i]; }
    //inline t       operator [](int i)            { return raw[i]; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
   // inline t       operator [](int i)            { return raw[i]; }
    t&             operator [](int i)            { return raw[i]; }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	float norm () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

Vec3f cross(Vec3f A, Vec3f B);

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}



class Matrix
{
public:
	Matrix();
	Matrix(const int r, const int c);
	std::vector<float>& operator [](const int i) { return m[i];};
	
	Matrix operator*(const Matrix &mat);
	//Matrix operator*();
	
	inline int nrows() {return rows;};
	inline int ncols() {return cols;};

	Vec3f toVec();
	Matrix transpose();

	Matrix inverse();

	static Matrix v2m(const Vec3f v);
	static Matrix identity(int dimensions = 4);
	static Matrix camLookAt(Vec3f eye, Vec3f target, Vec3f up);
	static Matrix viewport(int width, int height, int x, int y);
	int rows;
	int cols;
	
	std::vector<std::vector<float>> m;
	
	void print()
	{
		for(int i = 0; i<4;i++)
		{
			for( int j = 0; j <4; j++)
			{
				std::cout << m[i][j] << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};

Matrix v2m(Vec3f v);

#endif //__GEOMETRY_H__
