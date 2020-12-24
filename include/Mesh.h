#ifndef MESH_H
#define MESH_H
#include "Resource.h"


struct vertex {
	float x;
	float y;
	float z;
	float w;

	void print() {
		printf("(%f %f %f %f)\n",x,y,z,w);
	}

};

class Mesh: public Resource {

	public:
	
		std::vector<struct vertex> v_list;
		std::vector< std::vector < std::pair< long,long > > > f_list;
		std::vector<struct vertex> n_list;
		std::vector<std::string> mat_list;
		std::string name;
		float max = 1;		
		bool has_normals = false;

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
		void add_face(std::vector< std::pair< long,long > > face, std::string m);
		long polygons();
		bool _normals();
	
};	

#endif
