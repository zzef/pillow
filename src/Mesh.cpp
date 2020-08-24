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
			printf("%s\n",line.c_str());
			if (is_empty(line))
				continue;
			_split(line,line_p);
			if (line_p[0]=="usemtl") {
				//printf("material %s\n", line_p[1]);
				if (line_p.size()>1)
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
				std::vector<std::pair<long,long>> indices;
				for (int i = 1; i<line_p.size(); i++) {
					std::vector<std::string> index;
					std::string str(line_p[i]);
					int no_del = _split(str,index,'/');
					float ind = atol(index[0].c_str());
					float nind = atol(index[index.size()-1].c_str());
					if (index.size()<2 || no_del<2) {
						this->has_normals=false;
						nind = 0;
					}
					indices.push_back( std::make_pair(ind,nind) );
					std::vector<std::string> face_info;
					printf("%s <%s>\n",line_p[i].c_str(),index[0].c_str());
					//free up memory
				}
				indices.push_back(indices[0]);
				this->add_face(indices,curr_mat);
			}
			else if (line_p[0]=="vn") {
				this->has_normals=true;
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

bool Mesh::_normals() {
	return this->has_normals;
}

bool Mesh::reload() {
	return false;
}

void Mesh::display() {
	//this->print_mesh();
	
	for (int i = 0; i <n_list.size();i++) {
	
		n_list[i].print();
		
	}		

}

Mesh::~Mesh() {
	//TODO
}

int Mesh::type() {
	return 0;
}

void Mesh::add_normal(float x, float y, float z) {
	struct vertex n = {x ,y ,z, 0};

	printf("->adding %f %f %f %f\n",x,y,z,1.0f);

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

void Mesh::add_face(std::vector<std::pair<long,long>> face, std::string m) {
	this->f_list.push_back(face);
	//printf("face ");
	//for (int i = 0; i < face.size(); i++) {
		//printf("%i ",face[i]);
	//}
	//printf("\n");
	this->mat_list.push_back(m);
	printf("size f %d\n",this->mat_list.size());
}

long Mesh::polygons() {
	return f_list.size();
}

