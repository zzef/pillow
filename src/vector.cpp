#include "../include/includes.h"
#include "../include/vector.h"

Vec3::Vec3(float x, float y, float z) {
	this->x=x;
	this->y=y;
	this->z=z;
}

Vec3::Vec3() {
}


void Vec3::print() {
	printf("(%f %f %f)\n",this->x,this->y,this->z);
}

Vec3 Vec3::cross(Vec3 v) {			
	Vec3 n (
		(this->y * v.z) - (this->z * v.y),
		(this->z * v.x) - (this->x * v.z),
		(this->x * v.y) - (this->y * v.x)
	); 	
	return n;	
}

Vec3 Vec3::mid(Vec3 v) {
	
	Vec3 n (
		
		(this->x+v.x)/2,
		(this->y+v.y)/2,
		(this->z+v.z)/2
	
	);

	return n;

}

Vec3 Vec3::mul(float mag) {

	Vec3 n (
		(this->x*mag),
		(this->y*mag),
		(this->z*mag)
	);	
	return n;
}

Vec3 Vec3::normalize() {

	float norm = (float) (this->x*this->x)+(this->y*this->y)+(this->z*this->z);
	float invsqrt = (float) 1.0f/sqrtf(norm);

	Vec3 n (
		this->x*invsqrt,			
		this->y*invsqrt,			
		this->z*invsqrt			
	);
	return n;
	
}

Vec3 Vec3::add(Vec3 v) {
			
	Vec3 n (
		v.x+this->x,
		v.y+this->y,
		v.z+this->z
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

