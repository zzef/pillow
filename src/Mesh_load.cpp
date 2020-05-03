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

bool is_empty(std::string line) {
	if (line.length()<3)
		return true;
	return false;
}

void init_mat(struct mtl* m) {

	m->ka[0]=0.2f;
	m->ka[1]=0.2f;
	m->ka[2]=0.2f;

	m->ks[0]=1.0f;
	m->ks[1]=1.0f;
	m->ks[2]=1.0f;

	m->kd[0]=0.8f;
	m->kd[1]=0.8f;
	m->kd[2]=0.8f;

	m->Ns=30.0f;
	m->illum=2;

}

void load_model(std::string path, std::string mtl_path, std::vector<Mesh*>& models) {
	std::string line;

	std::map<std::string, struct mtl*> materials;
	
	std::ifstream mtl_file(mtl_path);		
	std::string material = "";
	struct mtl* mat = (struct mtl*) malloc(sizeof(struct mtl));
	init_mat(mat);
	materials["default"] = mat;
	
	if (mtl_file.is_open()) {	
		while(std::getline(mtl_file,line)) {
			std::vector <std::string>line_p;
			
			if (is_empty(line)) {
				//add sruct to map
				if (material != "") {
					materials[material]=mat;
					printf("material %s\n",material.c_str());
					printf("ka %f %f %f\n",mat->ka[0],mat->ka[1],mat->ka[2]);
					printf("ks %f %f %f\n",mat->ks[0],mat->ks[1],mat->ks[2]);
					printf("kd %f %f %f\n",mat->kd[0],mat->kd[1],mat->kd[2]);
					printf("Ns %f\n",mat->Ns);
					printf("illum %d\n",mat->illum);
					printf("\n");
				}
				continue;
			}

			_split(line,line_p);

			if (line_p[0] == "Ka") {
				mat->ka[0]=atof(line_p[1].c_str());				
				mat->ka[1]=atof(line_p[2].c_str());				
				mat->ka[2]=atof(line_p[3].c_str());				
			}

			if (line_p[0] == "Ks") {
				mat->ks[0]=atof(line_p[1].c_str());				
				mat->ks[1]=atof(line_p[2].c_str());				
				mat->ks[2]=atof(line_p[3].c_str());	
			}

			if (line_p[0] == "Kd") {
				mat->kd[0]=atof(line_p[1].c_str());				
				mat->kd[1]=atof(line_p[2].c_str());				
				mat->kd[2]=atof(line_p[3].c_str());	
			}

			if (line_p[0] == "Ns") {
				mat->Ns=atof(line_p[1].c_str());				
			}

			if (line_p[0] == "illum") {
				mat->illum=atoi(line_p[1].c_str());	
			}

			if (line_p[0] == "newmtl") {
				material = line_p[1];
				mat = (struct mtl*) malloc(sizeof(struct mtl));
				init_mat(mat);
			}	
		}	
	}

	std::ifstream model(path);
	if (model.is_open()) {
		Mesh *m = new Mesh(path);
		struct mtl* curr_mat;
		while (std::getline(model,line)) {
			std::vector <std::string>line_p;
			if (is_empty(line))
				continue;
			_split(line,line_p);
			if (line_p[0]=="usemtl") {
				curr_mat = materials[line_p[1]];	
			}
			else if (line_p[0]=="v") {
				m->add_vertex(
					atof(line_p[1].c_str()),
					atof(line_p[2].c_str()),
					atof(line_p[3].c_str())
				);
			}
			else if (line_p[0]=="f") {
				std::vector<long> indices;
				for (int i = 1; i<line_p.size(); i++) {
					std::vector<std::string> index;
					_split(line_p[i],index);
					indices.push_back(atol(index[0].c_str()));
					index.clear();
					index.shrink_to_fit();
					//free up memory
				}
				indices.push_back(indices[0]);
				m->add_face(indices,curr_mat);
			}
			else if (line_p[0]=="vn") {
				m->add_normal(
					atof(line_p[1].c_str()),
					atof(line_p[2].c_str()),
					atof(line_p[3].c_str())
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

