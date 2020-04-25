#include "vector.h"

Vec3::Vec3(float x, float y, float z) {
	this->x=x;
	this->y=y;
	this->z=z;
}

Vec3 Vec3::cross() {			
	Vec3 n (
		(this->y * v.z) - (this->z * v.y),
		(this->z * v.x) - (this->x * v.z),
		(this->x * v.y) - (this->y * v.x)
	); 	
	return n;	
}

Vec3 Vec3::res(Vec3 v) {
			
	Vec3 n (
		v.x-this->x,
		v.y-this->y,
		v.z-this->z
	);
	return n;
	
}		

float Vec3::dot(Vec3 v) {
	return (this->x*v.x) + (this->y*v.y) + (this->z*v.z);
}

