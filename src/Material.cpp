#include "../include/includes.h"
#include "../include/Resource.h"
#include "../include/utils.h"
#include "../include/Material.h"

int Material::type() {
	return 0;	
}

Material::Material() {
	mtl* mat = new mtl();
	init_mat(mat);
	this->mat_list["default"] = mat;
}

Material::~Material() {

}

bool Material::load(std::string path) {

	std::string line;
	std::ifstream mtl_file(path);		
	std::string material = "";
		
	if (mtl_file.is_open()) {	
		while(std::getline(mtl_file,line)) {
			std::vector <std::string>line_p;			
			
			if(is_empty(line))
				continue;		
	
			_split(line,line_p);	
			
			if (line_p[0] == "newmtl") {
				material = line_p[1];
				mtl* mat = new mtl();
				init_mat(mat);
				this->mat_list[material]=mat;
			}
			if (line_p[0] == "Ka") {
				this->mat_list[material]->ka[0]=atof(line_p[1].c_str());				
				this->mat_list[material]->ka[1]=atof(line_p[2].c_str());				
				this->mat_list[material]->ka[2]=atof(line_p[3].c_str());				
			}

			if (line_p[0] == "Ks") {
				this->mat_list[material]->ks[0]=atof(line_p[1].c_str());				
				this->mat_list[material]->ks[1]=atof(line_p[2].c_str());				
				this->mat_list[material]->ks[2]=atof(line_p[3].c_str());	
			}

			if (line_p[0] == "Kd") {
				this->mat_list[material]->kd[0]=atof(line_p[1].c_str());				
				this->mat_list[material]->kd[1]=atof(line_p[2].c_str());				
				this->mat_list[material]->kd[2]=atof(line_p[3].c_str());	
			}

			if (line_p[0] == "Ns") {
				this->mat_list[material]->Ns=atof(line_p[1].c_str());				
			}

			if (line_p[0] == "illum") {
				this->mat_list[material]->illum=atoi(line_p[1].c_str());	
			}
		}	
	}
	else {
		return false;
	}
	return true;
}

bool Material::reload() {
	return false;
}

void Material::display() {

	std::map<std::string, mtl*>::iterator it = this->mat_list.begin();
	
	while(it != this->mat_list.end()) {
		std::cout << it->first << std::endl;
		it->second->print();
		printf("\n");
		it++;
	}

}




