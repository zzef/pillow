#include "../include/includes.h"
#include "../include/vector.h"
#include "../include/Resource.h"
#include "../include/Mesh.h"
#include "../include/utils.h"

Mesh::Mesh() {

}

bool Mesh::load(std::string path) {

	std::string line;
	std::ifstream mesh(path);

	if (mesh.is_open()) {
		Mesh *m = new Mesh();
		std::string curr_mat;
		while (std::getline(mesh,line)) {
			std::vector <std::string>line_p;
			if (is_empty(line))
				continue;
			_split(line,line_p);
			if (line_p[0]=="usemtl") {
				//printf("material %s\n", line_p[1]);
				curr_mat = line_p[1];	
			}
			else if (line_p[0]=="v") {
				this->add_vertex(
					atof(line_p[1].c_str()),
					atof(line_p[2].c_str()),
					atof(line_p[3].c_str())
				);
			}
			else if (line_p[0]=="f") {
				std::vector<long> indices;
				for (int i = 1; i<line_p.size(); i++) {
					std::vector<std::string> index;
					std::string str(line_p[i]);
					_split(str,index);
					indices.push_back(atol(index[0].c_str()));
					//printf("<%s>\n",index[0].c_str());
					//free up memory
				}

				if(indices[0]==160 and indices[1]==56) {
					printf("found it!");
				}



				indices.push_back(indices[0]);
				this->add_face(indices,curr_mat);
			}
			else if (line_p[0]=="vn") {
				this->add_normal(
					atof(line_p[1].c_str()),
					atof(line_p[2].c_str()),
					atof(line_p[3].c_str())
				);
			}
		}
		mesh.close();
	}
	else {
		return false;
	}
	return true;

}

bool Mesh::reload() {
	return false;
}

void Mesh::display() {
	//this->print_mesh();
}

Mesh::~Mesh() {
	//TODO
}

int Mesh::type() {
	return 0;
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
	//printf("(%f %f %f %f)\n",x,y,z,1.0f);
	this->v_list.push_back(v);	
}

long Mesh::vertices() {
	return v_list.size();
}

void Mesh::add_face(std::vector<long> face, std::string m) {
	this->f_list.push_back(face);
	//printf("face ");
	//for (int i = 0; i < face.size(); i++) {
		//printf("%i ",face[i]);
	//}
	//printf("\n");
	this->mat_list.push_back(m);
}

long Mesh::polygons() {
	return f_list.size();
}

