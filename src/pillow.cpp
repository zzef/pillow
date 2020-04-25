#include "../include/includes.h"

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
	float x;
	struct Color c;
	float depth;
};

struct viewport {
	float x;
	float y;
	float w;
	float h;
};

const unsigned char fill = 50;
const unsigned char wf[3] = {100,100,100};
const unsigned char clc[3] = {30,30,30};

bool no_clipping=false;
long selected = 0;
const struct viewport vp = {150,50,800,600};
const float far = 30;
const float near = 2;
const int fov = 110;
const float S = 1/(tan((fov/2)*(M_PI/180)));
const float aspect_ratio = (float) WIN_WIDTH/WIN_HEIGHT;
const float pm[4][4] = {
	
	{ S/aspect_ratio, 0, 0, 0 },
	{ 0, S, 0, 0 },
	{ 0, 0,-(far/(far-near)),-1 },
	{ 0, 0,-(far*near)/(far-near), 0 }

};

std::vector<Mesh*> models;

unsigned char buffer[WIN_WIDTH][WIN_HEIGHT][4];
float depth_buffer[WIN_WIDTH][WIN_HEIGHT];
	
void clear_buffer() {
	for (int i = 0; i<WIN_WIDTH; i++) {
		for (int j = 0; j<WIN_HEIGHT; j++) {
			depth_buffer[i][j]=10000000000;	
		}
	}

	for (int i = 0; i<WIN_WIDTH; i++) {
		for (int j = 0; j<WIN_HEIGHT; j++) {
			buffer[i][j][0]=255;	
			buffer[i][j][1]=clc[0];
			buffer[i][j][2]=clc[1];
			buffer[i][j][3]=clc[2];
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

void set_pixel(int x, int y, unsigned char* color,float depth) {

	if (x<0 || x>=WIN_WIDTH) {
		//////printf("clipped x\n");
		return;
	}
	if (y<0 || y>=WIN_HEIGHT) {
		//////printf("clipped y\n");
		return;
	}

	float d = depth_buffer[x][y];
	if (depth<d) {
		depth_buffer[x][y]=depth;
	}
	else {
		return;
	}

	buffer[x][y][0]=color[3];
	buffer[x][y][1]=color[2];
	buffer[x][y][2]=color[1];
	buffer[x][y][3]=color[0];
}

void get_pairs(std::vector<struct vector3D> poly_r,
 	std::vector <std::vector<struct edge_pixel>>& edges,
	std::vector<struct Color> v_a, int min,int minx
) {

	for (int k = 0; k<poly_r.size()-1; k++) {

		unsigned char r = v_a[k].r;
		unsigned char g = v_a[k].g;
		unsigned char b = v_a[k].b;

		unsigned char r1 = v_a[k+1].r;
		unsigned char g1 = v_a[k+1].g;
		unsigned char b1 = v_a[k+1].b;

		float s_x = poly_r[k].x;
		float s_y = poly_r[k].y;
		float s_z = poly_r[k].z;
		float e_x = poly_r[k+1].x; 
		float e_y = poly_r[k+1].y;
		float e_z = poly_r[k+1].z;

		float sx = s_x;
		float sy = s_y;
		float ex = e_x; 
		float ey = e_y;
		float sz = s_z; 
		float ez = e_z;

		float dy = (float) ey-sy;
		float dx = (float) ex-sx;
		float dydx = (float) dy/dx;	
		float dxdy = 1/dydx;

		if (dydx>1 || dydx<-1) {
			
			float r_inc = (r1-r)/dy;
			float g_inc = (g1-g)/dy;
			float b_inc = (b1-b)/dy;

			if (s_y>e_y){
				sy=e_y;
				ey=s_y;
				sx=e_x;
				sz=e_z;
				ez=s_z;
			}
			float z_inc = (ez-sz)/(ey-sy);		
		
			float i = (float) sx;
			int inc = 0;
			for (float j = sy; j<ey; j++) {
				struct Color col = {
					r+(inc*r_inc),
					g+(inc*g_inc),
					b+(inc*b_inc)
				};
				float depth = sz+((j-sy)*z_inc);
				struct edge_pixel n = {i, col,depth};
				edges[(int)(j-min)][(int)(i-minx)]=n;	
				unsigned char c[4] = {wf[0],wf[1],wf[2],255};
				set_pixel((int)i,(int)j,c,1/depth);
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
				ey=s_y;
				sz=e_z;
				ez=s_z;
			}

			float z_inc = (ez-sz)/(ey-sy);		
			float j = (float) sy;
			int inc = 0;
			for (float i = sx; i<ex; i++) {
				struct Color col = {
					r+(inc*r_inc),
					g+(inc*g_inc),
					b+(inc*b_inc)
				};
				float depth = sz+((j-sy)*z_inc);
				struct edge_pixel n = {i,col,depth};
				edges[(int)(j-min)][(int)(i-minx)]=n;	
				unsigned char c[4] = {wf[0],wf[1],wf[2],255};
				set_pixel((int)i,(int)j,c,1/depth);
				j+=dydx;
				inc++;
			}
		}
	}
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

void render_triangle(int i, Mesh *m, std::vector <struct vertex>clip_coords) {

		std::vector <struct vertex*> new_poly;
		get_triangle(m->tf_list[i],new_poly,clip_coords);	
		//check dot product of camera to normal here and continue if negative
		
		if (new_poly.size()<1)
			return;

		struct vertex v0 = *new_poly[0];
		struct vertex v1 = *new_poly[1];
		struct vertex v2 = *new_poly[2];

		Vec3 vec0 (v0.x,v0.y,v0.w);
		Vec3 vec1 (v1.x,v1.y,v1.w);
		Vec3 vec2 (v2.x,v2.y,v2.w);

		Vec3 res1 = vec0.res(vec1);
		Vec3 res2 = vec0.res(vec2);

		Vec3 f_norm = res1.cross(res2);	
		
		Vec3 vec4 (0,0,0);
		Vec3 diff = vec4.res(vec0);

		if (f_norm.dot(diff)<0) 
			return;
	
		std::vector <struct vector3D> poly_r;		
		//PERSPECTIVE DIVIDE HERE
		int max = 0;
		int maxx = 0;
		int min = 100000000;
		int minx = 100000000;
		for (int k = 0; k<new_poly.size(); k++) {
			
			struct vertex *v = new_poly[k];

			float x = (v->x/v->w)*WIN_WIDTH + (WIN_WIDTH/2);
			float y = (-v->y/v->w)*WIN_HEIGHT + (WIN_HEIGHT/2);
			float z = v->z/v->w;
			
			struct vector3D r1 = {x,y,1/z};
			poly_r.push_back(r1);

			if (y>max) {
				max = y;
			}
			if (y<min) {
				min = y;
			}

			if (x>maxx) {
				maxx = x;
			}
			if (x<minx) {
				minx = x;
			}

		}	
		
		//RASTER SPACE
		std::vector <struct Color> vertex_attributes = {
				
			{fill,fill,fill},
			{fill,fill,fill},
			{fill,fill,fill},
			{fill,fill,fill},

		};
	
		const int range = max-min;	
		const int rangex = maxx-minx;	
		struct edge_pixel empty;
		empty.x=-1;
		std::vector <struct edge_pixel> v(rangex+2,empty);
		std::vector <std::vector<struct edge_pixel>> pairs(range+2,v);

		get_pairs(poly_r,pairs,vertex_attributes,min,minx);
		for (int l = 0; l<range+2; l++) {
			std::vector<struct edge_pixel> s = pairs[l];
			int yval = l+min;
			float smallest = 100000000;
			float largest = -100000000;
			int is = 0;
			int il = 0;
			for (int b = 0; b<s.size(); b++) {

				if (s[b].x==-1)
					continue;

				if (s[b].x < smallest) {
					smallest=s[b].x;
					is=b;
				}

				if (s[b].x > largest) {
					largest=s[b].x;
					il=b;
				}
			}
	
			if (s.size() < 1) {
				continue;
			}

			float first = smallest;
			float last = largest;
			
			float startz = s[is].depth;			
			float endz = s[il].depth;			
				
			float startr = s[is].c.r;			
			float startg = s[is].c.g;			
			float startb = s[is].c.b;			


			float r_inc = (s[il].c.r - s[is].c.r)/(last-first);
			float g_inc = (s[il].c.g - s[is].c.g)/(last-first);
			float b_inc = (s[il].c.b - s[is].c.b)/(last-first);
			float z_inc = (endz - startz)/(last-first);
		
			for (int n=first; n<last; n++) {
				if (s[n-minx].x != -1)
					continue;
				unsigned char r = startr+(r_inc*(n-first));
				unsigned char g = startg+(g_inc*(n-first));
				unsigned char b = startb+(b_inc*(n-first));
				float z = startz+(z_inc*(n-first));
				unsigned char color[4] = {
					r,g,b,255
				};
				set_pixel(n,yval,color,(1/z));
			}
		}

}

void render_mesh(Mesh *m) {

	unsigned char color[4] = {120,120,120,255};
	std::vector <struct vertex>clip_coords;
	float sf = 16;
	m->scale(sf,sf,sf);
	float tx = 0.0f;
	float ty = -8.0f;
	float tz = -22.0f;
	m->rotate_y(1.5f);
	m->translate(tx,ty,tz);

	for (int vtx = 0; vtx<m->vertices(); vtx++) {
		
		struct vertex* v = &(m->v_list[vtx]);
		//Applying matrix projection to enter clip space				
		float x = (float) ( v->x * pm[0][0] ) + ( v->y * pm[1][0] ) + ( v->z * pm[2][0] ) + ( v->w * pm[3][0] );
		float y = (float) ( v->x * pm[0][1] ) + ( v->y * pm[1][1] ) + ( v->z * pm[2][1] ) + ( v->w * pm[3][1] );
		float z = (float) ( v->x * pm[0][2] ) + ( v->y * pm[1][2] ) + ( v->z * pm[2][2] ) + ( v->w * pm[3][2] );
		float w = (float) ( v->x * pm[0][3] ) + ( v->y * pm[1][3] ) + ( v->z * pm[2][3] ) + ( v->w * pm[3][3] );	
		struct vertex c = {x,y,z,w};
		clip_coords.push_back(c);

	}

	for (int i = 0; i<m->triangles(); i++) {
		render_triangle(i,m,clip_coords);
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
	load_model("models/monkey.obj");
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
	load_model("models/Plane.obj");
	load_model("models/tugboat.obj");
	load_model("models/Suzuki_Carry.obj");
	load_model("models/snowcat.obj");
	load_model("models/car.obj");
	load_model("models/tractor.obj");
	load_model("models/treecartoon.obj");
	load_model("models/lighthouse.obj");
	load_model("models/rallycar.obj");
	load_model("models/voxel.obj");
	load_model("models/lowpolytree.obj");

	selected = 9;
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
	render_mesh(models[selected]);	

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
