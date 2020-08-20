#include "../include/includes.h"
#include "../include/Resource.h"
#include "../include/Mesh.h"
#include "../include/utils.h"
#include "../include/Material.h"
#include "../include/Model.h"
#include <algorithm>
#include <cmath>
#include <stdio.h>

Model::Model() {
	struct mtl* mat = (struct mtl*) malloc(sizeof(struct mtl));
	init_mat(mat);
	this->materials["default"] = mat;
}

Model::~Model() {
	this->cleanup_materials();
	//this->cleanup_faces();
	//this->cleanup_vertices();
}

void Model::cleanup_vertices() {
	for (int i = 0; i<vertices.size(); i++) {
		free(&this->vertices[i]);
	}
}

void Model::cleanup_faces() {
	for (int i = 0; i<faces.size(); i++) {
		free(&this->faces[i]);
	}
}

void Model::cleanup_materials() {
	
	std::map<std::string, struct mtl*>::iterator it = this->materials.begin();
	
	while(it != this->materials.end()) {
		std::cout << it->first << std::endl;
		free(it->second);
		it++;
	}
	
	materials.clear();

}

void Model::apply_attr(Material *material) {
	this->materials = material->mat_list;
	this->material = material;
	this->store_materials();
}

void Model::apply_attr(Mesh *mesh) {
	this->vertices = mesh->v_list;
	this->mesh=mesh;
	this->normalize();
	this->store_triangles();
}

void Model::update_max() {
	for (long i = 0; i<this->verts(); i++) {
		float x = abs(this->vertices[i].x);
		float y = abs(this->vertices[i].y);
		float z = abs(this->vertices[i].z);
		if (x>this->max)
			this->max=x;
		if (y>this->max)
			this->max=y;
		if (z>this->max)
			this->max=z;

	}
}

void Model::store_materials() {
	//assumes that all triangles are concave.	
	struct mtl* mat_p;
	std::string curr_material;

	if (!this->mesh)
		return;	

	this->mats.clear();
	for (int i = 0; i<this->mesh->polygons(); i++) {
		if (this->mesh->mat_list.size()-1>=i) {
			curr_material = this->mesh->mat_list[i];
			mat_p = this->materials[curr_material];
			if (mat_p == NULL) {
				mat_p = this->materials["default"];	
			}
		}
		else {
			mat_p = this->materials["default"];	
		}

		if (this->mesh->f_list[i].size()>4) {
			for (int j = 1; j<mesh->f_list[i].size()-2; j++) {
				this->mats.push_back(mat_p);
			}
		}
		else {
			this->mats.push_back(mat_p);
		}			
	}
}

void Model::store_triangles() {
	//assumes that all triangles are concave.	
	this->faces.clear();
	for (int i = 0; i<this->mesh->polygons(); i++) {
		struct face *fd0 = (struct face*) malloc(sizeof(struct face));
		if (this->mesh->f_list[i].size()>4) {
			for (int j = 1; j<mesh->f_list[i].size()-2; j++) {
				long v0 = mesh->f_list[i][0];
				long v1 = mesh->f_list[i][j];
				long v2 = mesh->f_list[i][j+1];
				fd0->v0 = &this->vertices[v0-1];
				fd0->v1 = &this->vertices[v1-1];
				fd0->v2 = &this->vertices[v2-1];	
				this->faces.push_back(*fd0);
				this->mats.push_back(this->materials["default"]);
			}
		}
		else {
			long v0 = mesh->f_list[i][0];
			long v1 = mesh->f_list[i][1];
			long v2 = mesh->f_list[i][2];
			fd0->v0 = &this->vertices[v0-1];
			fd0->v1 = &this->vertices[v1-1];
			fd0->v2 = &this->vertices[v2-1];
			this->faces.push_back(*fd0);
			this->mats.push_back(this->materials["default"]);
		}			
	}
}

void Model::normalize() {
	this->update_max();
	for (long i = 0; i<this->verts(); i++) {
		this->vertices[i].x/=this->max;
		this->vertices[i].y/=this->max;
		this->vertices[i].z/=this->max;
	}
}

long Model::verts() {
	return this->vertices.size();
}

long Model::triangles() {
	return this->faces.size();
}

void Model::scale(float x, float y, float z) {
		
	float scale_mat[4][4] = {
			
		{ x, 0, 0, 0 },
		{ 0, y, 0, 0 },
		{ 0, 0, z, 0 },
		{ 0, 0, 0, 1 }
	
	};
	
	this->apply_transform(scale_mat);
	
}

void Model::rotate_x(float angle) {

	float t = ((angle*M_PI)/180);
	float rot_x[4][4] = {
			
		{   1,      0,      0,      0   },
		{   0,   cos(t), -sin(t),   0   },
		{   0,   sin(t),  cos(t),   0   },
		{   0,      0,      0,      1   }
		
	};
	this->apply_transform(rot_x);

}

void Model::rotate_y(float angle) {

	float t = ((angle*M_PI)/180);
	float rot_y[4][4] = {
			
		{ cos(t),   0,    sin(t),   0   },
		{   0,      1,      0,      0   },
		{-sin(t),   0,    cos(t),   0   },
		{   0,      0,      0,      1   }
		
	};
	this->apply_transform(rot_y);

}

void Model::rotate_z(float angle) {

	float t = ((angle*M_PI)/180);
	float rot_z[4][4] = {
			
		{ cos(t),-sin(t),   0,      0   },
		{ sin(t), cos(t),   0,      0   },
		{   0,      0,      1,      0   },
		{   0,      0,      0,      1   }
		
	};
	this->apply_transform(rot_z);

}

void Model::translate(float x, float y, float z) {

	float trans[4][4] = {

		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ x, y, z, 1 }
	
	};
	this->apply_transform(trans);

}

void Model::apply_transform(float tm[4][4]) {

	for	(long i = 0; i<this->verts(); i++) {
	
		float x = ( this->vertices[i].x * tm[0][0] ) + ( this->vertices[i].y * tm[1][0] ) + ( this->vertices[i].z * tm[2][0] ) + ( this->vertices[i].w * tm[3][0] );
		float y = ( this->vertices[i].x * tm[0][1] ) + ( this->vertices[i].y * tm[1][1] ) + ( this->vertices[i].z * tm[2][1] ) + ( this->vertices[i].w * tm[3][1] );
		float z = ( this->vertices[i].x * tm[0][2] ) + ( this->vertices[i].y * tm[1][2] ) + ( this->vertices[i].z * tm[2][2] ) + ( this->vertices[i].w * tm[3][2] );
		float w = ( this->vertices[i].x * tm[0][3] ) + ( this->vertices[i].y * tm[1][3] ) + ( this->vertices[i].z * tm[2][3] ) + ( this->vertices[i].w * tm[3][3] );
		
		this->vertices[i].x = x;
		this->vertices[i].y = y;
		this->vertices[i].z = z;
		this->vertices[i].w = w;
	
	}

}

