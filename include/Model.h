#include "Material.h"
#include "Mesh.h"

struct face {
		
	struct vertex *v0;	
	struct vertex *v1;	
	struct vertex *v2;	

};

class Model {


	private:
		void cleanup_materials();
		void cleanup_faces();
		void cleanup_vertices();

	public:
		std::vector<struct vertex> vertices;
		std::map<std::string,struct mtl*> materials;
		std::string name;
		Material *material = 0;
		Mesh *mesh = 0;	
		float max = 1;		
		Model();
		~Model();

		std::vector<struct face> faces;
		std::vector<struct mtl*> mats;

		void apply_attr(Material *material);
		void apply_attr(Mesh *mesh);
		void update_max();
		void init();
		void store_triangles();
		void store_materials();
		void normalize();
		long verts();
		long triangles();
		void scale(float x, float y, float z);
		void rotate_x(float angle);
		void rotate_y(float angle);
		void rotate_z(float angle);
		void translate(float x, float y, float z);
		void apply_transform(float tm[4][4]);


};


