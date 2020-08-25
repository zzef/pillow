#ifndef MATERIAL_H
#define MATERIAL_H

#include "Resource.h"

struct mtl {

	float ka[3];
	float kd[3];
	float ks[3];
	int illum;
	float Ns;
	void print() {
			
		printf("Ka %f %f %f\n",ka[0],ka[1],ka[2]);
		printf("Kd %f %f %f\n",kd[0],kd[1],kd[2]);
		printf("Ks %f %f %f\n",ks[0],ka[1],ks[2]);
		printf("illum %d\n",illum);
		printf("Ns %f\n",Ns);

	}

};

class Material: public Resource {

	public:
		std::map<std::string, mtl*> mat_list;
		Material();	
		bool load(std::string path);
		bool reload();
		void display();
		int type();
		~Material(); //destructor

};

#endif
