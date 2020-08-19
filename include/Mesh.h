#ifndef MESH_H
#define MESH_H
#include "Resource.h"


struct vertex {
	float x;
	float y;
	float z;
	float w;
};

class Mesh: public Resource {

	public:
	
		std::vector<struct vertex> v_list;
		std::vector<std::vector<long>> f_list;
		std::vector<struct vector3D> n_list;
		std::vector<std::string> mat_list;
		std::string name;
		float max = 1;		

		Mesh();
		int type();
		bool load(std::string path);
		bool reload();
		void display();
		~Mesh();		

		void add_normal(float x, float y, float z);
		long normals();
		void add_vertex(float x, float y, float z);
		long vertices();
		void add_face(std::vector<long> face, std::string m);
		long polygons();
	
};	

#endif
