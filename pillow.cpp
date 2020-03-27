#include "SDL2/SDL.h"
#include "stdio.h"
#include "stdlib.h"
#include <time.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>

#define WIN_WIDTH 800
#define WIN_HEIGHT 800
#define RESOLUTION 1
#define WINDOW_TITLE "Pillow"

//Create frame buffer

struct vertex {
	float x;
	float y;
	float z;
	float w;
};

struct point2D {
	float x;
	float y;
};

const int fov = 110;
const float S = 1/(tan((fov/2)*(M_PI/180)));

const float pm[4][4] = {
	
	{ S, 0, 0, 0 },
	{ 0, S, 0, 0 },
	{ 0, 0,-1,-1 },
	{ 0, 0, 0, 0 }

};

class Camera {
	
	public:
		float transform[4][4] = {
			
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 1 }
 	
		};
	
};

class Mesh {

	public:
	
		std::vector<struct vertex> v_list;
		std::vector<std::vector<long>> f_list;
		std::string name;
		
		Mesh(std::string title) {
			this->name=title;
		}
	
		void add_vertex(float x, float y, float z) {
			struct vertex v = {x ,y ,z, 1};
			this->v_list.push_back(v);	
		}

		long vertices() {
			return v_list.size();
		}

		void add_face(std::vector<long> face) {
			this->f_list.push_back(face);
		}

		long faces() {
			return f_list.size();
		}

		void print_mesh() {
			std::cout<<name<<"\n";
			printf("===================\n");
			printf("vertices\n");
			printf("===================\n");
			for (int i = 0 ; i<this->v_list.size(); i++) {
				printf("%f %f %f %f\n", v_list[i].x, v_list[i].y, v_list[i].z, v_list[i].w);
			}
			printf("===================\n");
			printf("faces\n");
			printf("===================\n");
			for (int i = 0; i<this->f_list.size(); i++) {
				for (int j = 0; j<this->f_list[i].size(); j++) {
					printf("%ld ",f_list[i][j]);
				}
				printf("\n");
			}	
			printf("\n");	
		}

		void scale(float x, float y, float z) {
				
			float scale_mat[4][4] = {
					
				{ x, 0, 0, 0 },
				{ 0, y, 0, 0 },
				{ 0, 0, z, 0 },
				{ 0, 0, 0, 1 }
			
			};
			
			this->apply_transform(scale_mat);
			
		}
		
		void rotate_x(float angle) {

			float t = ((angle*M_PI)/180);
			float rot_x[4][4] = {
					
				{   1,      0,      0,      0   },
				{   0,   cos(t), -sin(t),   0   },
				{   0,   sin(t),  cos(t),   0   },
				{   0,      0,      0,      1   }
				
			};
			this->apply_transform(rot_x);

		}

		void rotate_y(float angle) {

			float t = ((angle*M_PI)/180);
			float rot_y[4][4] = {
					
				{ cos(t),   0,    sin(t),   0   },
				{   0,      1,      0,      0   },
				{-sin(t),   0,    cos(t),   0   },
				{   0,      0,      0,      1   }
				
			};
			this->apply_transform(rot_y);

		}

		void rotate_z(float angle) {

			float t = ((angle*M_PI)/180);
			float rot_z[4][4] = {
					
				{ cos(t),-sin(t),   0,      0   },
				{ sin(t), cos(t),   0,      0   },
				{   0,      0,      1,      0   },
				{   0,      0,      0,      1   }
				
			};
			this->apply_transform(rot_z);

		}

		void translate(float x, float y, float z) {

			float trans[4][4] = {

				{ 1, 0, 0, 0 },
				{ 0, 1, 0, 0 },
				{ 0, 0, 1, 0 },
				{ x, y, z, 1 }
			
			};
			this->apply_transform(trans);

		}
		
		void apply_transform(float tm[4][4]) {

			for	(long i = 0; i<this->vertices(); i++) {
			
				this->v_list[i].x = ( this->v_list[i].x * tm[0][0] ) + ( this->v_list[i].y * tm[1][0] ) + ( this->v_list[i].z * tm[2][0] ) + ( this->v_list[i].w * tm[3][0] );
				this->v_list[i].y = ( this->v_list[i].x * tm[0][1] ) + ( this->v_list[i].y * tm[1][1] ) + ( this->v_list[i].z * tm[2][1] ) + ( this->v_list[i].w * tm[3][1] );
				this->v_list[i].z = ( this->v_list[i].x * tm[0][2] ) + ( this->v_list[i].y * tm[1][2] ) + ( this->v_list[i].z * tm[2][2] ) + ( this->v_list[i].w * tm[3][2] );
				this->v_list[i].w = ( this->v_list[i].x * tm[0][3] ) + ( this->v_list[i].y * tm[1][3] ) + ( this->v_list[i].z * tm[2][3] ) + ( this->v_list[i].w * tm[3][3] );

			}
	
		}
	
};	

std::vector<Mesh*> models;


void split(const std::string& str, std::vector<std::string>& vec, char delim = ' ') {
	std::istringstream iss(str);
	std::string token;
	for(std::string s; iss >> s; ) {
		vec.push_back(s);
	}
}

void load_model(std::string path) {
	std::string line;
	std::ifstream model(path);
	if (model.is_open()) {
		Mesh *m = new Mesh(path);
		while (std::getline(model,line)) {
			if (line[0]=='v' && line[1]==' ') {
				std::vector<std::string> vertex;
				split(line,vertex);
				m->add_vertex(
					atof(vertex[1].c_str()),
					atof(vertex[2].c_str()),
					atof(vertex[3].c_str())
				);
			}
			else if (line[0]=='f' && line[1]==' ') {
				std::vector<std::string> face;
				split(line,face);
				std::vector<long> indices;
				for (int i = 1; i<face.size(); i++) {
					std::vector<std::string> index;
					split(face[i],index);
					indices.push_back(atol(index[0].c_str()));
					index.clear();
					index.shrink_to_fit();
					//free ip memory
				}
				m->add_face(indices);
			}
		}
		models.push_back(m);
		model.close();
	}
	else {
		std::cout << "Unable to open file\n";
	}
}

unsigned char buffer[WIN_WIDTH][WIN_HEIGHT][4];
	
void clear_buffer() {
	for (int i = 0; i<WIN_WIDTH; i++) {
		for (int j = 0; j<WIN_HEIGHT; j++) {
			buffer[i][j][0]=255;	
			buffer[i][j][1]=30;
			buffer[i][j][2]=30;
			buffer[i][j][3]=30;
		}
	}
}

template<char r1, char c1, char r2, char c2>
void mat_mul(float m1[r1][c1], float m2[r2][c2], char *rows2, char *columns2, float*** mr) {

	if (c1 != r2) {
		//printf("not possible to multiply %dx%d and %dx%d.\n",
			//r1,c1,r2,c2
		//);
		return;
	}
	
	float** res = (float**) malloc(sizeof(float**)*r1);
	for (int i = 0; i < r1; i++) {
		res[i]=(float*)malloc(sizeof(float*)*c1);
	}
	
	for (int i = 0; i<r1; i++) {
		for (int j = 0; j<c2; j++) {	
			float sum = 0;
			for (int k = 0; k<c1; k++) {
				////printf("%f x %f, ",m1[i][k],m2[k][j]);
				sum+=(m1[i][k]*m2[k][j]);
			}	
			////printf("\n====\n");
			res[i][j]=sum;
		}
	}

	*rows2=r1;
	*columns2=c2;
	*mr = res;
	
}



void flip_buffer(SDL_Renderer* renderer, SDL_Texture* Frame) {
	unsigned char* bytes = nullptr;
	int pitch = 0;
	//get locked pixels and store in bytes for write access
	SDL_LockTexture(Frame,nullptr,(void**)(&bytes),&pitch);
	for (int y = 0; y < WIN_HEIGHT; y++) {
		for (int x = 0; x < WIN_WIDTH; x++) {
			memcpy(&bytes[(y*WIN_WIDTH+x)*4],buffer[x][y],4);
		}
	}
	SDL_UnlockTexture(Frame);
	SDL_Rect destination = {0,0,WIN_WIDTH,WIN_HEIGHT};
	SDL_RenderCopy(renderer,Frame,NULL,&destination);		
	//show
	SDL_RenderPresent(renderer);
}

void set_pixel(int x, int y, unsigned char* color) {
	if (x<0 || x>=WIN_WIDTH) {
		////printf("clipped x\n");
		return;
	}
	if (y<0 || y>=WIN_HEIGHT) {
		////printf("clipped y\n");
		return;
	}
	buffer[x][y][0]=color[3];
	buffer[x][y][1]=color[2];
	buffer[x][y][2]=color[1];
	buffer[x][y][3]=color[0];
}

void optimized_bresenham(int s_x, int s_y, int e_x, int e_y, unsigned char* color)  {

	int sx = s_x;
	int sy = s_y;
	int ex = e_x; 
	int ey = e_y;

	int dy = ey-sy;
	int dx = ex-sx;

	if (abs(dy)>abs(dx)) {
		if (s_y>e_y){
			sy=e_y;
			ey=s_y;
			sx=e_x;
		}

		int threshold = dy;
		int threshold_inc = dy*2;
		int delta = dx*2;
		int i = sx;
		int accum = 0;
		for (int j = sy; j<ey; j++) {
			set_pixel(i,j,color);
			accum+=delta;
			if (accum>=threshold) {
				i++;
				threshold+=threshold_inc;
			}
		}
	}
	else {
		if (s_x>e_x){
			sx=e_x;
			ex=s_x;
			sy=e_y;
		}

		int threshold = dx;
		int threshold_inc = dx*2;
		int delta = dy*2;
		int j = sy;
		int accum = 0;
		for (int i = sx; i<ex; i++) {
			set_pixel(i,j,color);
			accum+=delta;
			if (accum>=threshold) {
				j++;
				threshold+=threshold_inc;
			}
		}
	}

}


void naive_bresenham(int s_x, int s_y, int e_x, int e_y, unsigned char* color)  {

	int sx = (int) s_x;
	int sy = (int) s_y;
	int ex = (int) e_x; 
	int ey = (int) e_y;

	float dy = (float) ey-sy;
	float dx = (float) ex-sx;
	float dydx = (float) dy/dx;	
	float dxdy = 1/dydx;
	
	//printf("gradient %f\n",dydx);
	
	bool debug = false;
	if ((sx==1024 && sy==0) && (ex==768 && ey==192)) {
		debug=true;
		//printf("-=====================================\n");
	}
	
		//printf("drawing line (%i,%i) -> (%i,%i)\n",sx,sy,ex,ey);
	
	if (dydx>1 || dydx<-1) {
			//printf("here===================\n");
		if (s_y>e_y){
			sy=e_y;
			ey=s_y;
			sx=e_x;
		}
			//printf("starting with (%i,%i)\n",sx,sy);
	
		float i = (float) sx;
		for (int j = sy; j<ey; j++) { 
			set_pixel((int)i,(int)j,color);
			//printf("%f + %f",i,dxdy);
			i+=dxdy;
			//printf(" = %f\n",i);
			//printf("setting pixel (%i,%i)\n",i,j);
		}
	}
	else {
			//printf("ehere=================\n");
		if (s_x>e_x){
			sx=e_x;
			ex=s_x;
			sy=e_y;
		}
		float j = (float) sy;
		for (int i = sx; i<ex; i++) {
			set_pixel((int)i,(int)j,color);
			//printf("%f + %f",j,dydx);
			j+=dydx;
			//printf(" = %f\n",j);
			//printf("setting pixel (%i,%i)\n",(int)i,(int)j);
		}
	}

}

void draw_line(int s_x, int s_y, int e_x, int e_y, unsigned char* color) {
	naive_bresenham(s_x,s_y,e_x,e_y,color);
}

void initialize() {

	load_model("models/Tree low.obj");
	load_model("models/Gel Gun.obj");
	load_model("models/WindMill.obj");
	load_model("models/cube.obj");
	load_model("models/Plane.obj");


	models[3]->print_mesh();	

	for (int i = 0; i<models.size(); i++) {
		models[i]->print_mesh();
	}
	
	float m34t[3][4] = {
		{4,33,2,7},
		{5.3,6,6,3},
		{8,6,3,6.6}
	};
	float m42t[4][2] = {
		{4,13},
		{5.3,7},
		{5.5,6},
		{8,6}
	};


	float **ptr;
	char rows = 0;
	char columns = 0;	

	mat_mul<3,4,4,2>(m34t,m42t,&rows,&columns,&ptr);
	m34t[2][1]=8.8;

	for (int i = 0; i<rows; i++) {
		for (int j = 0; j<columns; j++) {
			//printf("%f ",ptr[i][j]);
		}
		//printf("\n");
	}

	mat_mul<3,4,4,2>(m34t,m42t,&rows,&columns,&ptr);
	for (int i = 0; i<rows; i++) {
		for (int j = 0; j<columns; j++) {
			//printf("%f ",ptr[i][j]);
		}
		//printf("\n");
	}

}

void render_mesh(Mesh *m, Camera *camera) {

	unsigned char color[4] = {120,120,120,255};
	std::vector <struct point2D>verts;
	//printf("==================\n");
	float sf = 1;
	m->scale(sf,sf,sf);
	float transz = -6.0f;
	m->rotate_y(-0.8f);
	m->translate(0,0,transz);
	
	for (struct vertex v : m->v_list ) {
				
		float x = (float) ( v.x * pm[0][0] ) + ( v.y * pm[1][0] ) + ( v.z * pm[2][0] ) + ( v.w * pm[3][0] );
		float y = (float) ( v.x * pm[0][1] ) + ( v.y * pm[1][1] ) + ( v.z * pm[2][1] ) + ( v.w * pm[3][1] );
		float z = (float) ( v.x * pm[0][2] ) + ( v.y * pm[1][2] ) + ( v.z * pm[2][2] ) + ( v.w * pm[3][2] );
		float w = (float) ( v.x * pm[0][3] ) + ( v.y * pm[1][3] ) + ( v.z * pm[2][3] ) + ( v.w * pm[3][3] );
		//should do clipping at this point apparently	
		struct point2D s = {x/w,y/w};
		s.x=(s.x*WIN_WIDTH)+WIN_WIDTH/2;
		s.y=(-s.y*WIN_HEIGHT)+WIN_HEIGHT/2;
		verts.push_back(s);
	}

	for (struct point2D s: verts) {
		set_pixel(s.x,s.y,color);
	}

	//printf("all verts new\n");
	for (int i = 0; i<verts.size(); i++) {
		//printf("%f %f\n",verts[i].x,verts[i].y);
	}

	for (int i = 0 ; i<m->faces(); i++) {
		for (int j = 0; j<m->f_list[i].size()-1; j++) {
	
				
			long sv = m->f_list[i][j];
			long ev = m->f_list[i][j+1];
	
			float sx = verts[sv-1].x;
			float sy = verts[sv-1].y;
			float ex = verts[ev-1].x;
			float ey = verts[ev-1].y;
			
			draw_line(sx,sy,ex,ey,color);			
	

			//printf("edge from %ld to %ld - (%f,%f) -> (%f,%f)\n",sv,ev,sx,sy,ex,ey);	
		}

		long sv = m->f_list[i][0];	
		long ev = m->f_list[i][m->f_list[i].size()-1];
	
		float sx = verts[sv-1].x;
		float sy = verts[sv-1].y;
		float ex = verts[ev-1].x;
		float ey = verts[ev-1].y;
			
		draw_line(sx,sy,ex,ey,color);			


		//printf("edge from %ld to %ld - (%f,%f) -> (%f,%f)\n",sv,ev,sx,sy,ex,ey);	
	}
	m->translate(0,0,-transz);
	m->scale(1/sf,1/sf,1/sf);
}

void render() {

	unsigned char color[4] = {120,120,120,255};
	unsigned char red[4] = {255,0,0,255};
	
	int m = 3;

	//for (int i = 0; i < 10000; i++) {
	//	draw_line(rand() % WIN_WIDTH,rand() % WIN_HEIGHT,rand() % WIN_WIDTH,rand() % WIN_HEIGHT,color);
	//}
	//draw_line(10,10,454,333,red);
	Camera *cam = new Camera();
	render_mesh(models[m],cam);	

}

void update() {
	//TODO
}

int main(int argc, char* args[]) {
	
	//Start SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//We start by creating a window	
	SDL_Window *window = SDL_CreateWindow(
		WINDOW_TITLE,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WIN_WIDTH,
		WIN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	//We need a renderer to do our rendering
	SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,0);
	//Set the clear color for our renderer
	SDL_SetRenderDrawColor(renderer,30,30,30,255);
	//clear
	SDL_RenderClear(renderer);
	
	//Create frame
	SDL_Texture *Frame = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING, WIN_WIDTH,WIN_HEIGHT
	);
	int frames = 0;
	clock_t before = clock();
	initialize();
	//return 0;
	while(1) {
		clear_buffer();
		if((clock() - before) / CLOCKS_PER_SEC > 1) {
			printf("fps %i\n",frames);
			before = clock();
			frames=0;
		}
		update();
		render();
		flip_buffer(renderer,Frame);
		frames++;
	}
	//Quit SDL
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
