#include "SDL2/SDL.h"
#include "stdio.h"
#include "stdlib.h"
#include <vector>
#include <array>
#include <time.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <math.h>

#define WIN_WIDTH 1024
#define WIN_HEIGHT 768
#define RESOLUTION 1
#define WINDOW_TITLE "Pillow"

//Create frame buffer

struct vertex {
	float x;
	float y;
	float z;
	float w;
};

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct edge_pixel {	
	int x;
	struct Color c;
};

struct viewport {
	float x;
	float y;
	float w;
	float h;
};

struct vector3D {
	float x;
	float y;
	float z;
};

bool no_clipping=false;
long selected = 0;
const struct viewport vp = {150,50,800,600};
const float far = 10;
const float near = 1;
const int fov = 110;
const float S = 1/(tan((fov/2)*(M_PI/180)));
const float aspect_ratio = (float) WIN_WIDTH/WIN_HEIGHT;
const float pm[4][4] = {
	
	{ S/aspect_ratio, 0, 0, 0 },
	{ 0, S, 0, 0 },
	{ 0, 0,-(far/(far-near)),-1 },
	{ 0, 0,-(far*near)/(far-near), 0 }

};

class Camera {
	
	public:
		float transform[4][4] = {
			
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 1 }
 	
		};
	struct vector3D eye_point() {
		struct vector3D ep = {0,0,0};
		return ep;
	}
	
};

class Vec3 {

	public:
		float x;
		float y;
		float z;
		
		Vec3(float x, float y, float z) {
			this->x=x;
			this->y=y;
			this->z=z;
		}

		Vec3* cross(Vec3 *v) {
			
			Vec3 *n = new Vec3(
				(this->y * v->z) - (this->z * v->y),
				(this->z * v->x) - (this->x * v->z),
				(this->x * v->y) - (this->y * v->x)
			); 	
			return n;	
			
		}

		Vec3* res(Vec3 *v) {
			
			Vec3 *n = new Vec3(
				v->x-this->x,
				v->y-this->y,
				v->z-this->z
			);
			return n;
	
		}		

		float dot(Vec3 *v) {
			return (this->x*v->x) + (this->y*v->y) + (this->z*v->z);
		}

};

class Mesh {

	public:
	
		std::vector<struct vertex> v_list;
		std::vector<std::vector<long>> f_list;
		std::vector<std::vector<long>> tf_list;
		std::vector<struct vector3D> n_list;
		std::string name;
		float max = 1;		

		Mesh(std::string title) {
			this->name=title;
		}

		void update_max() {
			for (long i = 0; i<this->vertices(); i++) {
				if (this->v_list[i].x>max)
					max=this->v_list[i].x;
				if (this->v_list[i].y>max)
					max=this->v_list[i].y;
				if (this->v_list[i].z>max)
					max=this->v_list[i].z;

			}
		}

		void triangulate() {
			//assumes that all triangles are concave.	
			for (int i = 0; i<this->polygons(); i++) {
				if (f_list[i].size()>4) {
					for (int j = 1; j<f_list[i].size()-2; j++) {
						std::vector<long> v;
						long v0 = f_list[i][0];
						long v1 = f_list[i][j];
						long v2 = f_list[i][j+1];
						v.push_back(v0);
						v.push_back(v1);
						v.push_back(v2);
						v.push_back(v0);
						this->tf_list.push_back(v);
					}
				}
				else {
					tf_list.push_back(f_list[i]);
				}
			}
		}

		void normalize() {
			this->update_max();
			for (long i = 0; i<this->vertices(); i++) {
				this->v_list[i].x/=max;
				this->v_list[i].y/=max;
				this->v_list[i].z/=max;
			}
		}

		void add_normal(float x, float y, float z) {
			struct vector3D n = {x ,y ,z};
			this->n_list.push_back(n);	
		}

		long normals() {
			return n_list.size();
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

		long polygons() {
			return f_list.size();
		}


		long triangles() {
			return tf_list.size();
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
			printf("triangles\n");
			printf("===================\n");
			for (int i = 0; i<this->f_list.size(); i++) {
				for (int j = 0; j<this->f_list[i].size(); j++) {
					printf("%ld ",f_list[i][j]);
				}
				printf("\n");
			}
			printf("===================\n");
			printf("normals\n");
			printf("===================\n");
			for (int i = 0 ; i<this->n_list.size(); i++) {
				printf("%f %f %f\n", n_list[i].x, n_list[i].y, n_list[i].z);
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
			
				float x = ( this->v_list[i].x * tm[0][0] ) + ( this->v_list[i].y * tm[1][0] ) + ( this->v_list[i].z * tm[2][0] ) + ( this->v_list[i].w * tm[3][0] );
				float y = ( this->v_list[i].x * tm[0][1] ) + ( this->v_list[i].y * tm[1][1] ) + ( this->v_list[i].z * tm[2][1] ) + ( this->v_list[i].w * tm[3][1] );
				float z = ( this->v_list[i].x * tm[0][2] ) + ( this->v_list[i].y * tm[1][2] ) + ( this->v_list[i].z * tm[2][2] ) + ( this->v_list[i].w * tm[3][2] );
				float w = ( this->v_list[i].x * tm[0][3] ) + ( this->v_list[i].y * tm[1][3] ) + ( this->v_list[i].z * tm[2][3] ) + ( this->v_list[i].w * tm[3][3] );
				
				this->v_list[i].x = x;
				this->v_list[i].y = y;
				this->v_list[i].z = z;
				this->v_list[i].w = w;
			
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
					//free up memory
				}
				indices.push_back(indices[0]);
				m->add_face(indices);
			}
			else if (line[0]=='v' && line[1]=='n') {
				std::vector<std::string> normal;
				split(line,normal);
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

unsigned char buffer[WIN_WIDTH][WIN_HEIGHT][4];
	
void clear_buffer() {
	for (int i = 0; i<WIN_WIDTH; i++) {
		for (int j = 0; j<WIN_HEIGHT; j++) {
			buffer[i][j][0]=255;	
			buffer[i][j][1]=200;
			buffer[i][j][2]=200;
			buffer[i][j][3]=200;
		}
	}
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
		//////printf("clipped x\n");
		return;
	}
	if (y<0 || y>=WIN_HEIGHT) {
		//////printf("clipped y\n");
		return;
	}
	buffer[x][y][0]=color[3];
	buffer[x][y][1]=color[2];
	buffer[x][y][2]=color[1];
	buffer[x][y][3]=color[0];
}

void get_pairs(std::vector<struct vector3D> poly_r,
 	std::vector <std::vector<struct edge_pixel>>& edges,
	std::vector<struct Color> v_a, int min
) {

	for (int k = 0; k<poly_r.size()-1; k++) {

		unsigned char r = v_a[k].r;
		unsigned char g = v_a[k].g;
		unsigned char b = v_a[k].b;

		unsigned char r1 = v_a[k+1].r;
		unsigned char g1 = v_a[k+1].g;
		unsigned char b1 = v_a[k+1].b;

		int s_x = (int) poly_r[k].x;
		int s_y = (int) poly_r[k].y;
		int e_x = (int) poly_r[k+1].x; 
		int e_y = (int) poly_r[k+1].y;

		int sx = (int) s_x;
		int sy = (int) s_y;
		int ex = (int) e_x; 
		int ey = (int) e_y;

		float dy = (float) ey-sy;
		float dx = (float) ex-sx;
		float dydx = (float) dy/dx;	
		float dxdy = 1/dydx;
		float abdy = abs(dy);
		float abdx = abs(dx);	
	
		if (dydx>1 || dydx<-1) {
			
			float r_inc = (r1-r)/dy;
			float g_inc = (g1-g)/dy;
			float b_inc = (b1-b)/dy;
		
			if (s_y>e_y){
				sy=e_y;
				ey=s_y;
				sx=e_x;
			}
		
			float i = (float) sx;
			int inc = 0;
			for (int j = sy; j<ey; j++) {
				struct Color col = {
					r+(inc*r_inc),
					g+(inc*g_inc),
					b+(inc*b_inc)
				};
				struct edge_pixel n = {(int) i, col};
				edges[(int)(j-min)].push_back(n);	
				unsigned char c[4] = {col.r,col.g,col.b,255};
				set_pixel((int)i,(int)j,c);
				i+=dxdy;
				inc++;
			}
		}
		else {

			float r_inc = (r1-r)/dx;
			float g_inc = (g1-g)/dx;
			float b_inc = (b1-b)/dx;
		
			if (s_x>e_x){
				sx=e_x;
				ex=s_x;
				sy=e_y;
			}
			float j = (float) sy;
			int inc = 0;
			for (int i = sx; i<ex; i++) {
				struct Color col = {
					r+(inc*r_inc),
					g+(inc*g_inc),
					b+(inc*b_inc)
				};
				struct edge_pixel n = {(int) i,col};
				edges[(int)(j-min)].push_back(n);	
				unsigned char c[4] = {col.r,col.g,col.b,255};
				set_pixel((int)i,(int)j,c);
				j+=dydx;
				inc++;
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
	
	////printf("gradient %f\n",dydx);
	
	bool debug = false;
	if ((sx==1024 && sy==0) && (ex==768 && ey==192)) {
		debug=true;
		////printf("-=====================================\n");
	}
	
		////printf("drawing line (%i,%i) -> (%i,%i)\n",sx,sy,ex,ey);
	
	if (dydx>1 || dydx<-1) {
			////printf("here===================\n");
		if (s_y>e_y){
			sy=e_y;
			ey=s_y;
			sx=e_x;
		}
			////printf("starting with (%i,%i)\n",sx,sy);
	
		float i = (float) sx;
		for (int j = sy; j<ey; j++) { 
			set_pixel((int)i,(int)j,color);
			////printf("%f + %f",i,dxdy);
			i+=dxdy;
			////printf(" = %f\n",i);
			////printf("setting pixel (%i,%i)\n",i,j);
		}
	}
	else {
			////printf("ehere=================\n");
		if (s_x>e_x){
			sx=e_x;
			ex=s_x;
			sy=e_y;
		}
		float j = (float) sy;
		for (int i = sx; i<ex; i++) {
			set_pixel((int)i,(int)j,color);
			////printf("%f + %f",j,dydx);
			j+=dydx;
			////printf(" = %f\n",j);
			////printf("setting pixel (%i,%i)\n",(int)i,(int)j);
		}
	}

}



void draw_line(int s_x, int s_y, int e_x, int e_y, unsigned char* color) {
	naive_bresenham(s_x,s_y,e_x,e_y,color);
}

void draw_rect(float x, float y, float w, float h, unsigned char* color) {
	draw_line(x,y,x+w,y,color);
	draw_line(x+w,y,x+w,y+h,color);
	draw_line(x+w,y+h,x,y+h,color);
	draw_line(x,y+h,x,y,color);
}

float dot3D (struct vector3D v1, struct vector3D v2) {
	return (v1.x*v2.x)+(v1.y*v2.y)+(v1.z*v2.z);
}

void clip_triangle(std::vector<long>& ply, std::vector<struct vertex> &new_poly, 
	std::vector<struct vertex>& clip_coords
) {	
	for (int j = 0; j<ply.size()-1; j++) {	
	
		long sv = ply[j];
		long ev = ply[j+1];
	
		struct vertex* s = &clip_coords[sv-1];
		struct vertex* e = &clip_coords[ev-1];

		if (no_clipping) {
			struct vertex n = {e->x,e->y,e->z,e->w};
			struct vertex n1 = {s->x,s->y,s->z,s->w};
			new_poly.push_back(n);		
			continue;
		}
		if (s->z>s->w && e->z>e->w)
			continue;

		// WILL I EVER BE ABLE TO LOOK BACK AT THIS AND UNDERSTAND IT?
		// PROBABLY NOT. AND THAT IS SAD.

		//Implementation of Liang Barsky line clipping <- (okay this should remind you, otherwise, you're screwed)
		if (e->z<=e->w && s->z>=s->w) {
			float t = (s->w-e->z)/(s->z-e->z);
			float nx = e->x+(t*(s->x-e->x));
			float ny = e->y+(t*(s->y-e->y));
			struct vertex n = {nx,ny,s->w,s->w};
			struct vertex n1 = {e->x,e->y,e->z,e->w};
			new_poly.push_back(n);
			new_poly.push_back(n1);
		}
		else if (s->z<=s->w && e->z>=e->w) {
			float t = (e->w-s->z)/(e->z-s->z);
			float nx = s->x+(t*(e->x-s->x));
			float ny = s->y+(t*(e->y-s->y));
			struct vertex n = {nx,ny,e->w,e->w};
			//struct vertex n1 = {s->x,s->y,s->z,s->w};
			//new_poly.push_back(n1);		
			new_poly.push_back(n);
		}
		else {
			struct vertex n = {e->x,e->y,e->z,e->w};
			//struct vertex n1 = {s->x,s->y,s->z,s->w};
			new_poly.push_back(n);		
		}

	}

	new_poly.push_back(new_poly[0]);

}

void get_triangle(std::vector<long>& ply, std::vector<struct vertex*> &new_poly, 
	std::vector<struct vertex>& clip_coords
) {	
	for (int j = 0; j<ply.size(); j++) {
	
		long sv = ply[j];	
		struct vertex* s = &clip_coords[sv-1];
		//get normal here and put it inside;
		new_poly.push_back(s);		
		
	}
	
}	

void render_mesh(Mesh *m, Camera *camera) {

	unsigned char color[4] = {120,120,120,255};
	std::vector <struct vertex>clip_coords;
	////printf("==================\n");
	float sf = 1.6;
	m->scale(sf,sf,sf);
	float tx = 0.0f;
	float ty = -0.3f;
	float tz = -2.5f;
	m->rotate_y(1.5f);
	m->translate(tx,ty,tz);

	for (struct vertex v : m->v_list ) {

		//Applying matrix projection to enter clip space				
		float x = (float) ( v.x * pm[0][0] ) + ( v.y * pm[1][0] ) + ( v.z * pm[2][0] ) + ( v.w * pm[3][0] );
		float y = (float) ( v.x * pm[0][1] ) + ( v.y * pm[1][1] ) + ( v.z * pm[2][1] ) + ( v.w * pm[3][1] );
		float z = (float) ( v.x * pm[0][2] ) + ( v.y * pm[1][2] ) + ( v.z * pm[2][2] ) + ( v.w * pm[3][2] );
		float w = (float) ( v.x * pm[0][3] ) + ( v.y * pm[1][3] ) + ( v.z * pm[2][3] ) + ( v.w * pm[3][3] );	
		struct vertex c = {x,y,z,w};
		clip_coords.push_back(c);

	}

	for (int i = 0; i<m->triangles(); i++) {
		std::vector <struct vertex*> new_poly;
		get_triangle(m->tf_list[i],new_poly,clip_coords);	
		//check dot product of camera to normal here and continue if negative
		
		if (new_poly.size()<1)
			continue;

		struct vertex v0 = *new_poly[0];
		struct vertex v1 = *new_poly[1];
		struct vertex v2 = *new_poly[2];

		Vec3 *vec0 = new Vec3(v0.x,v0.y,v0.w);
		Vec3 *vec1 = new Vec3(v1.x,v1.y,v1.w);
		Vec3 *vec2 = new Vec3(v2.x,v2.y,v2.w);

		Vec3 *res1 = vec0->res(vec1);
		Vec3 *res2 = vec0->res(vec2);

		Vec3 *f_norm = res1->cross(res2);	
		
		Vec3 *vec4 = new Vec3(0,0,0);
		Vec3 *diff = vec4->res(vec0);

		if (f_norm->dot(diff)<0) 
			continue;
	
		std::vector <struct vector3D> poly_r;		
		//PERSPECTIVE DIVIDE HERE
		int max = 0;
		int min = 100000000;
		for (int k = 0; k<new_poly.size(); k++) {
			
			struct vertex *v = new_poly[k];
			float sx = (v->x/v->w)*WIN_WIDTH + (WIN_WIDTH/2);
			float sy = (-v->y/v->w)*WIN_HEIGHT + (WIN_HEIGHT/2);
			
			struct vector3D r1 = {(v->x/v->w)*WIN_WIDTH + (WIN_WIDTH/2),(-v->y/v->w)*WIN_HEIGHT + (WIN_HEIGHT/2),v->z};
			poly_r.push_back(r1);

			if (sy>max) {
				max = sy;
			}
			if (sy<min) {
				min = sy;
			}

			//printf("(%f,%f) -> (%f,%f)\n",sx,sy,ex,ey);
			//printf("drawing line between (%f,%f) and (%f,%f)\n",sx,sy,ex,ey);

		}	
		
		//RASTER SPACE
		std::vector <struct Color> vertex_attributes = {
			{180,180,180},
			{180,180,180},
			{180,180,180},
			{180,180,180}
		};
	
		const int range = max-min;	
		std::vector <std::vector<struct edge_pixel>> pairs(range+1);
		get_pairs(poly_r,pairs,vertex_attributes,min);
		for (int l = 0; l<range; l++) {
			std::vector<struct edge_pixel> s = pairs[l];
			
			if (s.size() < 1) {
				continue;
			}
			int yval = l+min;
			for (int m = 0; m<s.size(); m++) {

				int first = s[m].x;
				int last = s[s.size()-1].x;
				
				if (first > last) {
					int temp = first;
					first = last;
					last = temp;
				}
				
				int startr = s[m].c.r;			
				int startg = s[m].c.g;			
				int startb = s[m].c.b;			

				if (last-first <1)
					continue;

				int r_inc = (int) (s[s.size()-1].c.r - s[m].c.r)/(last-first);
				int g_inc = (int) (s[s.size()-1].c.g - s[m].c.g)/(last-first);
				int b_inc = (int) (s[s.size()-1].c.b - s[m].c.b)/(last-first);
				
				for (int n = first; n<last; n++) {
				
					
					unsigned char r = startr+(r_inc*(n-first));
					unsigned char g = startg+(g_inc*(n-first));
					unsigned char b = startb+(b_inc*(n-first));

					unsigned char color[4] = {
						r,g,b,255
					};
				
					set_pixel(n,yval,color);
				}

			}
			
			//struct edge_pixel x2 = pairs[i][pairs[i].size()-1];
		}

		//for wireframe
		for (int j = 0; j<poly_r.size()-1; j++) {
			struct vector3D v = poly_r[j];
			struct vector3D v1 = poly_r[j+1];
			draw_line(v.x,v.y,v1.x,v1.y,color);
		}
	}
	

	m->translate(-tx,-ty,-tz);
	m->scale(1/sf,1/sf,1/sf);
}

void initialize() {

	load_model("models/Tree low.obj");
	load_model("models/Gel Gun.obj");
	load_model("models/WindMill.obj");
	load_model("models/cube.obj");
	load_model("models/Love.obj");
	load_model("models/low-poly-mill.obj");
	load_model("models/suzanne.obj");
	load_model("models/camera.obj");
	load_model("models/Lowpoly_tree_sample.obj");
	load_model("models/vehicle.obj");
	load_model("models/Jeep_Renegade_2016.obj");
	load_model("models/house_plant.obj");
	load_model("models/boat.obj");
	load_model("models/casa.obj");
	load_model("models/well.obj");
	load_model("models/crow.obj");
	load_model("models/tank.obj");
	load_model("models/drill.obj");

	selected = 5;
	models[selected]->normalize();
	models[selected]->triangulate();
	models[selected]->print_mesh();
	printf("model: %s, triangles: %li, vertices: %li\n",
		models[selected]->name.c_str(),
		models[selected]->triangles(),
		models[selected]->vertices()
	);


	
}

void render() {

	unsigned char color[4] = {120,120,120,255};
	unsigned char red[4] = {255,0,0,255};

	//for (int i = 0; i < 10000; i++) {
	//	draw_line(rand() % WIN_WIDTH,rand() % WIN_HEIGHT,rand() % WIN_WIDTH,rand() % WIN_HEIGHT,color);
	//}
	//draw_line(10,10,454,333,red);
	Camera *cam = new Camera();
	//render_mesh(models[0],cam);	
	//render_mesh(models[1],cam);	
	//render_mesh(models[2],cam);	
	render_mesh(models[selected],cam);	
	//render_mesh(models[6],cam);	
	//render_mesh(models[5],cam);	

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
