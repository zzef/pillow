#include "../include/includes.h"
#include "../include/vector.h"
#include "../include/Mesh.h"

Mesh::Mesh(std::string title) {
	this->name=title;
}

void Mesh::update_max() {
	for (long i = 0; i<this->vertices(); i++) {
		if (this->v_list[i].x>max)
			max=this->v_list[i].x;
		if (this->v_list[i].y>max)
			max=this->v_list[i].y;
		if (this->v_list[i].z>max)
			max=this->v_list[i].z;

	}
}

void Mesh::triangulate() {
	//assumes that all triangles are concave.	
	for (int i = 0; i<this->polygons(); i++) {
		if (f_list[i].size()>4) {
			for (int j = 1; j<f_list[i].size()-2; j++) {
				std::vector<long> v;
				long v0 = f_list[i][0];
				long v1 = f_list[i][j];
				long v2 = f_list[i][j+1];
				v.push_back(v0);
				v.push_back(v1);
				v.push_back(v2);
				v.push_back(v0);
				this->tf_list.push_back(v);
			}
		}
		else {
			tf_list.push_back(f_list[i]);
		}
	}
}

void Mesh::normalize() {
	this->update_max();
	for (long i = 0; i<this->vertices(); i++) {
		this->v_list[i].x/=max;
		this->v_list[i].y/=max;
		this->v_list[i].z/=max;
	}
}

void Mesh::add_normal(float x, float y, float z) {
	struct vector3D n = {x ,y ,z};
	this->n_list.push_back(n);	
}

long Mesh::normals() {
	return n_list.size();
}


void Mesh::add_vertex(float x, float y, float z) {
	struct vertex v = {x ,y ,z, 1};
	this->v_list.push_back(v);	
}

long Mesh::vertices() {
	return v_list.size();
}

void Mesh::add_face(std::vector<long> face) {
	this->f_list.push_back(face);
}

long Mesh::polygons() {
	return f_list.size();
}


long Mesh::triangles() {
	return tf_list.size();
}

void Mesh::print_mesh() {
	std::cout<<name<<"\n";
	printf("===================\n");
	printf("vertices\n");
	printf("===================\n");
	for (int i = 0 ; i<this->v_list.size(); i++) {
		printf("%f %f %f %f\n", v_list[i].x, v_list[i].y, v_list[i].z, v_list[i].w);
	}
	printf("===================\n");
	printf("triangles\n");
	printf("===================\n");
	for (int i = 0; i<this->f_list.size(); i++) {
		for (int j = 0; j<this->f_list[i].size(); j++) {
			printf("%ld ",f_list[i][j]);
		}
		printf("\n");
	}
	printf("===================\n");
	printf("normals\n");
	printf("===================\n");
	for (int i = 0 ; i<this->n_list.size(); i++) {
		printf("%f %f %f\n", n_list[i].x, n_list[i].y, n_list[i].z);
	}

	printf("\n");
}

void Mesh::scale(float x, float y, float z) {
		
	float scale_mat[4][4] = {
			
		{ x, 0, 0, 0 },
		{ 0, y, 0, 0 },
		{ 0, 0, z, 0 },
		{ 0, 0, 0, 1 }
	
	};
	
	this->apply_transform(scale_mat);
	
}

void Mesh::rotate_x(float angle) {

	float t = ((angle*M_PI)/180);
	float rot_x[4][4] = {
			
		{   1,      0,      0,      0   },
		{   0,   cos(t), -sin(t),   0   },
		{   0,   sin(t),  cos(t),   0   },
		{   0,      0,      0,      1   }
		
	};
	this->apply_transform(rot_x);

}

void Mesh::rotate_y(float angle) {

	float t = ((angle*M_PI)/180);
	float rot_y[4][4] = {
			
		{ cos(t),   0,    sin(t),   0   },
		{   0,      1,      0,      0   },
		{-sin(t),   0,    cos(t),   0   },
		{   0,      0,      0,      1   }
		
	};
	this->apply_transform(rot_y);

}

void Mesh::rotate_z(float angle) {

	float t = ((angle*M_PI)/180);
	float rot_z[4][4] = {
			
		{ cos(t),-sin(t),   0,      0   },
		{ sin(t), cos(t),   0,      0   },
		{   0,      0,      1,      0   },
		{   0,      0,      0,      1   }
		
	};
	this->apply_transform(rot_z);

}

void Mesh::translate(float x, float y, float z) {

	float trans[4][4] = {

		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ x, y, z, 1 }
	
	};
	this->apply_transform(trans);

}

void Mesh::apply_transform(float tm[4][4]) {

	for	(long i = 0; i<this->vertices(); i++) {
	
		float x = ( this->v_list[i].x * tm[0][0] ) + ( this->v_list[i].y * tm[1][0] ) + ( this->v_list[i].z * tm[2][0] ) + ( this->v_list[i].w * tm[3][0] );
		float y = ( this->v_list[i].x * tm[0][1] ) + ( this->v_list[i].y * tm[1][1] ) + ( this->v_list[i].z * tm[2][1] ) + ( this->v_list[i].w * tm[3][1] );
		float z = ( this->v_list[i].x * tm[0][2] ) + ( this->v_list[i].y * tm[1][2] ) + ( this->v_list[i].z * tm[2][2] ) + ( this->v_list[i].w * tm[3][2] );
		float w = ( this->v_list[i].x * tm[0][3] ) + ( this->v_list[i].y * tm[1][3] ) + ( this->v_list[i].z * tm[2][3] ) + ( this->v_list[i].w * tm[3][3] );
		
		this->v_list[i].x = x;
		this->v_list[i].y = y;
		this->v_list[i].z = z;
		this->v_list[i].w = w;
	
	}

}

