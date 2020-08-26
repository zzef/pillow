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
	mtl* mat = new mtl();
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

long Model::polys() {
	return this->polygons;
}

void Model::apply_attr(Mesh *mesh) {
	this->polygons = mesh->polygons();
	this->vertices = mesh->v_list;
	this->normals = mesh->n_list;
	this->mesh=mesh;
	this->update_data();
	this->center_model();
	this->normalize();
	this->store_triangles();
	this->_normals = mesh->_normals();
}

void Model::center_model() {
	
	float mid_x = (this->max_x + this->min_x)/2;
	float mid_y = (this->max_y + this->min_y)/2;
	float mid_z = (this->max_z + this->min_z)/2;

	this->max_x-=mid_x;
	this->max_y-=mid_y;
	this->max_z-=mid_z;

	this->min_x-=mid_x;
	this->min_y-=mid_y;
	this->min_z-=mid_z;
	
	for (long i = 0; i<this->verts(); i++) {
		this->vertices[i].x-=mid_x;
		this->vertices[i].y-=mid_y;
		this->vertices[i].z-=mid_z;
	}

}

bool Model::has_normals() {
	return this->_normals;
}

void Model::update_data() {
	
	float x = this->vertices[0].x;
	float y = this->vertices[0].y;
	float z = this->vertices[0].z;
	this->max_x=x;
	this->max_y=y;
	this->max_z=z;
	this->min_x=x;
	this->min_y=y;
	this->min_z=z;

	for (long i = 1; i<this->verts(); i++) {
		x=this->vertices[i].x;
		y=this->vertices[i].y;
		z=this->vertices[i].z;
		if (x>this->max_x)
			this->max_x=x;
		if (y>this->max_y)
			this->max_y=y;
		if (z>this->max_z)
			this->max_z=z;
		if (x<this->min_x)
			this->min_x=x;
		if (y<this->min_y)
			this->min_y=y;
		if (z<this->min_z)
			this->min_z=z;

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
		face *fd0 = new face();
		if (this->mesh->f_list[i].size()>4) {
			for (int j = 1; j<mesh->f_list[i].size()-2; j++) {
				long v0 = mesh->f_list[i][0].first;
				long v1 = mesh->f_list[i][j].first;
				long v2 = mesh->f_list[i][j+1].first;
				long vn0 = mesh->f_list[i][0].second;
				long vn1 = mesh->f_list[i][j].second;
				long vn2 = mesh->f_list[i][j+1].second;
				fd0->v0 = &this->vertices[v0-1];
				fd0->v1 = &this->vertices[v1-1];
				fd0->v2 = &this->vertices[v2-1];
				if (this->normals.size()>0) {
					fd0->n0 = &this->normals[vn0-1];
					fd0->n1 = &this->normals[vn1-1];
					fd0->n2 = &this->normals[vn2-1];	
				}
				this->faces.push_back(*fd0);
				this->mats.push_back(this->materials["default"]);
			}
		}
		else {
			long v0 = mesh->f_list[i][0].first;
			long v1 = mesh->f_list[i][1].first;
			long v2 = mesh->f_list[i][2].first;
			long vn0 = mesh->f_list[i][0].second;
			long vn1 = mesh->f_list[i][1].second;
			long vn2 = mesh->f_list[i][2].second;
			fd0->v0 = &this->vertices[v0-1];
			fd0->v1 = &this->vertices[v1-1];
			fd0->v2 = &this->vertices[v2-1];
			if (this->normals.size()>0) {
				fd0->n0 = &this->normals[vn0-1];
				fd0->n1 = &this->normals[vn1-1];
				fd0->n2 = &this->normals[vn2-1];	
			}
			this->faces.push_back(*fd0);
			this->mats.push_back(this->materials["default"]);
		}			
	}
}

void Model::normalize() {
	
	float max1 = std::max(std::max(abs(max_x),abs(max_y)),abs(max_z));
	float max2 = std::max(std::max(abs(min_x),abs(min_y)),abs(min_z));
	float max = std::max(max1,max2);	

	for (long i = 0; i<this->verts(); i++) {
		this->vertices[i].x/=max;
		this->vertices[i].y/=max;
		this->vertices[i].z/=max;
	}
}

long Model::norms() {
	return this->normals.size();
}

long Model::verts() {
	return this->vertices.size();
}

long Model::tris() {
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
	//this->apply_transformn(scale_mat);
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
	this->apply_transformn(rot_x);

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
	this->apply_transformn(rot_y);

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
	this->apply_transformn(rot_z);

}

void Model::translate(float x, float y, float z) {

	float trans[4][4] = {

		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ x, y, z, 1 }
	
	};
	this->apply_transform(trans);
	//this->apply_transformn(trans);

}

void Model::apply_transformn(float tm[4][4]) {

	if (!this->_normals)
		return;

	for	(long i = 0; i<this->normals.size(); i++) {
	
		float x = ( this->normals[i].x * tm[0][0] ) + ( this->normals[i].y * tm[1][0] ) + ( this->normals[i].z * tm[2][0] ) + ( this->normals[i].w * tm[3][0] );
		float y = ( this->normals[i].x * tm[0][1] ) + ( this->normals[i].y * tm[1][1] ) + ( this->normals[i].z * tm[2][1] ) + ( this->normals[i].w * tm[3][1] );
		float z = ( this->normals[i].x * tm[0][2] ) + ( this->normals[i].y * tm[1][2] ) + ( this->normals[i].z * tm[2][2] ) + ( this->normals[i].w * tm[3][2] );
		float w = ( this->normals[i].x * tm[0][3] ) + ( this->normals[i].y * tm[1][3] ) + ( this->normals[i].z * tm[2][3] ) + ( this->normals[i].w * tm[3][3] );
		
		this->normals[i].x = x;
		this->normals[i].y = y;
		this->normals[i].z = z;
		this->normals[i].w = w;
	
	}

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

