#ifndef VEC3_H
#define VEC3_H

struct vector3D {
	float x;
	float y;
	float z;
};

struct vector2D {
	int x;
	int y;
};

class Vec3 {

	public:
		float x;
		float y;
		float z;
		
		Vec3(float x, float y, float z);
		Vec3 cross(Vec3 v);
		Vec3 res(Vec3 v);
		float dot(Vec3 v);
		Vec3 normalize();
		Vec3 mid(Vec3 v);
		Vec3 mul(float mag);

};

#endif
