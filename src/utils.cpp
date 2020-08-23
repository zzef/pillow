#include "../include/includes.h"
#include "../include/Material.h"

void _split(const std::string& str, std::vector<std::string>& vec, char delim = ' ') {
	//printf("->%s\n",str.c_str());
	std::string curr = "";
	std::string::size_type i = 0;
	while(1) {	
		char chr = str[i];		
		//printf("%c\n",chr);	
		if (chr=='\r'||chr=='\n'||chr==0) {
			//printf("breaking out\n");
			if (curr.length()>0)
				vec.push_back(curr);
			break;
		}
		if (chr==delim) {
			if (curr.length()>0) {
				//printf("sdsdsd\n");
				//printf("adding %s\n",curr.c_str());
				vec.push_back(curr);
				curr="";
			}
		}
		else {
			curr+=chr;	
		}
		i++;
	}
	//printf("\n");

}

bool is_empty(std::string line) {
	if (line.length()<3)
		return true;
	return false;
}

void init_mat(struct mtl* m) {

	m->ka[0]=0.5f;
	m->ka[1]=0.5f;
	m->ka[2]=0.5f;

	m->ks[0]=0.0f;
	m->ks[1]=0.0f;
	m->ks[2]=0.0f;

	m->kd[0]=0.5f;
	m->kd[1]=0.5f;
	m->kd[2]=0.5f;

	m->Ns=30.0f;
	m->illum=2;

}

