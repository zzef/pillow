#include "../include/includes.h"
#include "../include/Material.h"

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

