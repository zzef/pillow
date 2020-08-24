#include "../include/includes.h"
#include "../include/Material.h"

int _split(const std::string& str, std::vector<std::string>& vec, char delim = ' ') {
	//printf("->%s\n",str.c_str());
	std::string curr = "";
	std::string::size_type i = 0;
	int no_del = 0;
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
			no_del++;
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
	return no_del;
}

bool is_empty(std::string line) {
	if (line.length()<3)
		return true;
	return false;
}

void init_mat(struct mtl* m) {

	m->ka[0]=0.7f;
	m->ka[1]=0.7f;
	m->ka[2]=0.7f;

	m->ks[0]=0.6f;
	m->ks[1]=0.6f;
	m->ks[2]=0.6f;

	m->kd[0]=0.5f;
	m->kd[1]=0.5f;
	m->kd[2]=0.5f;

	m->Ns=60.0f;
	m->illum=2;

}

