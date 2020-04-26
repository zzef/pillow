#include "../include/includes.h"
#include "../include/Mesh.h"
#include "../include/Mesh_load.h"
#include "../include/vector.h"
#include "../include/display.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
#define RESOLUTION 1
#define WINDOW_TITLE "Pillow"

//Create frame buffer

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct edge_pixel {	
	int x;
	struct Color c;
	float depth;
};

struct viewport {
	float x;
	float y;
	float w;
	float h;
};

const unsigned char fill = 70;
const unsigned char wf[3] = {120,120,120};
unsigned char clear_color[4] = {40,40,40,255};
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
Display* display;

void get_pairs(std::vector<struct vector3D> poly_r,
 	std::vector <std::vector<struct edge_pixel>>& edges,
	std::vector<struct Color> v_a, int min,int minx
) {

	printf("newting\n");
	for (int k = 0; k<poly_r.size()-2; k++) {

		float r = (float) v_a[k].r;
		float g = (float) v_a[k].g;
		float b = (float) v_a[k].b;

		float r1 = (float) v_a[k+1].r;
		float g1 = (float) v_a[k+1].g;
		float b1 = (float) v_a[k+1].b;

		printf("(%f,%f,%f) -> (%f,%f,%f)\n",r,g,b,r1,g1,b1);
		
		int x0 = (int)poly_r[k].x;
		int y0 = (int)poly_r[k].y;
		float z0 = poly_r[k].z;
		int x1 = (int)poly_r[k+1].x; 
		int y1 = (int)poly_r[k+1].y;
		float z1 = poly_r[k+1].z;

		int dx = abs(x1-x0);
		int dy = abs(y1-y0);

		int sx = (x0 < x1) ? 1 : -1;
		int sy = (y0 < y1) ? 1 : -1;
		
		int err = dx - dy;
		std::vector<struct vector2D> raster;
		unsigned char c[4] = {wf[0],wf[1],wf[2],255};	
		while(true) {
			//struct edge_pixel n = {x0, col,depth};
			struct vector2D point = {x0,y0};
			raster.push_back(point);
			
			if ((x0==x1) && (y0==y1)) break;
			int e2 = 2 * err;
			if (e2 > -dy) { err -= dy; x0 += sx; }
			if (e2 < dx) { err += dx; y0 += sy; }
		}
		
		int no_points = raster.size();

		float delta_r = r1-r;
		float delta_g = g1-g;
		float delta_b = b1-b;
		float delta_z = z1-z0;
		
		//printf("deltas (%f,%f,%f)\n",delta_r,delta_g,delta_b);

		for (int i = 0; i<no_points; i++) {
			float prop = (float) i/no_points;
			int x = raster[i].x;
			int y = raster[i].y;
			/*
			printf("-------------------\n");
			printf("(%f,", r + (prop*delta_r));
			printf("%f,", g + (prop*delta_g));
			printf("%f)\n", b + (prop*delta_b));
			printf("(%i,", (unsigned char) (r + (prop*delta_r)));
			printf("%i,", (unsigned char) (g + (prop*delta_g)));
			printf("%i)\n", (unsigned char) (b + (prop*delta_b)));
			printf("-------------------\n");
			*/
			struct Color col = {
				(unsigned char) (r + (prop*delta_r)),
				(unsigned char) (g + (prop*delta_b)),
				(unsigned char) (b + (prop*delta_g))
			};
			float depth = z0 + (prop*delta_z);
			struct edge_pixel n = {x,col,depth};
			edges[y-min][x-minx]=n;
			unsigned char c[4] = {col.r,col.g,col.b,255};	
			display->set_pixel(x,y,c,1/depth);
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
		
			{255,0,0},
			{0,255,0},
			{0,0,255},
			{255,0,0}
		
			//{fill,fill,fill},
			//{fill,fill,fill},
			//{fill,fill,fill},
			//{fill,fill,fill},

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
				
			float startr = (float) s[is].c.r;			
			float startg = (float) s[is].c.g;			
			float startb = (float) s[is].c.b;			


			float r_inc = (s[il].c.r - s[is].c.r)/(last-first);
			float g_inc = (s[il].c.g - s[is].c.g)/(last-first);
			float b_inc = (s[il].c.b - s[is].c.b)/(last-first);
			float z_inc = (endz - startz)/(last-first);
		
			for (int n=first; n<last; n++) {
				if (s[n-minx].x != -1)
					continue;
				unsigned char r = (unsigned char) (startr+(r_inc*(n-first)));
				unsigned char g = (unsigned char) (startg+(g_inc*(n-first)));
				unsigned char b = (unsigned char) (startb+(b_inc*(n-first)));
				float z = startz+(z_inc*(n-first));
				unsigned char color[4] = {
					r,g,b,255
				};
				display->set_pixel(n,yval,color,(1/z));
			}
		}

}

void render_mesh(Mesh *m) {

	unsigned char color[4] = {120,120,120,255};
	std::vector <struct vertex>clip_coords;
	float sf = 12;
	m->scale(sf,sf,sf);
	float tx = 0.0f;
	float ty = -3.0f;
	float tz = -22.0f;
	m->rotate_y(1.5f);
	m->translate(tx,ty,tz);

	for (int vtx = 0; vtx<m->vertices(); vtx++) {
		
		struct vertex* v = &(m->v_list[vtx]);
		//Applying matrix projection to enter clip space				
		float x = ( v->x * pm[0][0] ) + ( v->y * pm[1][0] ) + ( v->z * pm[2][0] ) + ( v->w * pm[3][0] );
		float y = ( v->x * pm[0][1] ) + ( v->y * pm[1][1] ) + ( v->z * pm[2][1] ) + ( v->w * pm[3][1] );
		float z = ( v->x * pm[0][2] ) + ( v->y * pm[1][2] ) + ( v->z * pm[2][2] ) + ( v->w * pm[3][2] );
		float w = ( v->x * pm[0][3] ) + ( v->y * pm[1][3] ) + ( v->z * pm[2][3] ) + ( v->w * pm[3][3] );	
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

	//load_model("models/Tree low.obj",models);
	//load_model("models/Gel Gun.obj",models);
	//load_model("models/WindMill.obj",models);
	load_model("models/cube.obj",models);
	//load_model("models/Love.obj",models);
	//load_model("models/low-poly-mill.obj",models);
	//load_model("models/suzanne.obj",models);
	//load_model("models/monkey.obj",models);
	//load_model("models/camera.obj",models);
	//load_model("models/Lowpoly_tree_sample.obj",models);
	//load_model("models/vehicle.obj",models);
	//load_model("models/Jeep_Renegade_2016.obj",models);
	//load_model("models/house_plant.obj",models);
	//load_model("models/boat.obj",models);
	//load_model("models/casa.obj",models);
	//load_model("models/well.obj",models);
	//load_model("models/crow.obj",models);
	//load_model("models/tank.obj",models);
	//load_model("models/drill.obj",models);
	//load_model("models/Plane.obj",models);
	//load_model("models/tugboat.obj",models);
	//load_model("models/Suzuki_Carry.obj",models);
	//load_model("models/snowcat.obj",models);
	//load_model("models/car.obj",models);
	//load_model("models/tractor.obj",models);
	//load_model("models/treecartoon.obj",models);
	//load_model("models/lighthouse.obj",models);
	//load_model("models/rallycar.obj",models);
	//load_model("models/voxel.obj",models);
	//load_model("models/lowpolytree.obj",models);

	selected = 0;
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
	
	display = new Display(WIN_WIDTH,WIN_HEIGHT,WINDOW_TITLE);
	display->init();
	display->set_clear_color(clear_color);
	int frames = 0;
	clock_t before = clock();
	initialize();
	while(1) {
		display->clear_buffer();
		if((clock() - before) / CLOCKS_PER_SEC > 1) {
			printf("fps %i\n",frames);
			before = clock();
			frames=0;
		}
		update();
		render();
		display->flip_buffer();
		frames++;
	}
	display->destroy();

	return 0;
}
