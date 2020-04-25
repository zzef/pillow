#include "../include/includes.h"
#include "../include/Mesh.h"
#include "../include/Mesh_load.h"

void _split(const std::string& str, std::vector<std::string>& vec, char delim = ' ') {
	std::istringstream iss(str);
	std::string token;
	for(std::string s; iss >> s; ) {
		vec.push_back(s);
	}
}

void load_model(std::string path, std::vector<Mesh*>& models) {
	std::string line;
	std::ifstream model(path);
	if (model.is_open()) {
		Mesh *m = new Mesh(path);
		while (std::getline(model,line)) {
			if (line[0]=='v' && line[1]==' ') {
				std::vector<std::string> vertex;
				_split(line,vertex);
				m->add_vertex(
					atof(vertex[1].c_str()),
					atof(vertex[2].c_str()),
					atof(vertex[3].c_str())
				);
			}
			else if (line[0]=='f' && line[1]==' ') {
				std::vector<std::string> face;
				_split(line,face);
				std::vector<long> indices;
				for (int i = 1; i<face.size(); i++) {
					std::vector<std::string> index;
					_split(face[i],index);
					indices.push_back(atol(index[0].c_str()));
					index.clear();
					index.shrink_to_fit();
					//free up memory
				}
				indices.push_back(indices[0]);
				m->add_face(indices);
			}
			else if (line[0]=='v' && line[1]=='n') {
				std::vector<std::string> normal;
				_split(line,normal);
				m->add_normal(
					atof(normal[1].c_str()),
					atof(normal[2].c_str()),
					atof(normal[3].c_str())
				);
			}

		}
		models.push_back(m);
		model.close();
	}
	else {
		std::cout << "Unable to open file\n";
	}
}

