#ifndef MESH_H
#define MESH_H

class Mesh {

	public:
	
		std::vector<struct vertex> v_list;
		std::vector<std::vector<long>> f_list;
		std::vector<std::vector<long>> tf_list;
		std::vector<struct vector3D> n_list;
		std::string name;
		float max = 1;		

		Mesh(std::string title);
		void update_max();
		void triangulate();
		void normalize();
		void add_normal(float x, float y, float z);
		long normals();
		void add_vertex(float x, float y, float z);
		long vertices();
		void add_face(std::vector<long> face);
		long polygons();
		long triangles();
		void print_mesh();
		void scale(float x, float y, float z);
		void rotate_x(float angle);
		void rotate_y(float angle);
		void rotate_z(float angle);
		void translate(float x, float y, float z);
		void apply_transform(float tm[4][4]);
	
};	

#endif
