#include "../include/includes.h"
#include "../include/Mesh.h"
#include "../include/Model.h"
#include "../include/Material.h" 
#include "../include/Camera.h"
#include "../include/vector.h"
#include "../include/display.h"
#include "../include/utils.h"

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

bool draw_vertex = false;
bool backface_culling = true;
bool draw_wireframe = false;
bool draw_lights = false;
bool flat_shade = false;
struct vector2D curr_raster[(WIN_HEIGHT*2)+(WIN_WIDTH*2)];
struct edge_pixel edge_pixels[WIN_HEIGHT][WIN_WIDTH];
unsigned char pc[4] = {255,255,255,255};					
const unsigned char wf[3] = {40,40,40};					
const unsigned char wf2[3] = {255,255,0};					
unsigned char wfc[4] = {wf[0],wf[1],wf[2],255};
unsigned char clear_color[4] = {220,220,220,255};
bool no_clipping=false;
long selected = 0;
const struct viewport vp = {150,50,800,600};
const float far = 20;
const float near = 1;
const int fov = 90;
const float S = 1/(tan((fov/2)*(M_PI/180)));
const float aspect_ratio = (float) WIN_WIDTH/WIN_HEIGHT;
struct edge_pixel empty = {-1,NULL,0};
float projection_matrix[4][4] = {
	
	{ S/aspect_ratio, 0, 0, 0 },
	{ 0, S, 0, 0 },
	{ 0, 0,-(far/(far-near)),-1 },
	{ 0, 0,-(far*near)/(far-near), 0 }

};


//lighting
float light_move=0;
const int lights = 5;
float ambient_light[3] = {(float)(70.0f/255.0f),(float)(70.0f/255.0f),(float)(70.0f/255.0f)};
float point_light[lights][6] = {

	{(float)(200.0f/255.0f),(float)(200.0f/255.0f),(float)(200.0f/255.0f),0,3,5}, 
	{(float)(160.0f/255.0f),(float)(160.0f/255.0f),(float)(160.0f/255.0f),0,5,-5}, 
	{(float)(100.0f/255.0f),(float)(100.0f/255.0f),(float)(100.0f/255.0f),0,10,0},
	{(float)(160.0f/255.0f),(float)(160.0f/255.0f),(float)(160.0f/255.0f),20,4,-5},
	{(float)(190.0f/255.0f),(float)(190.0f/255.0f),(float)(190.0f/255.0f),-20,4,-5},

}; 

std::vector<Model*> models;
Display* display;


void get_pairs(struct vector3D poly_r[4],
	struct Color v_a[4], int min,int minx
) {

	for (int k = 0; k<3; k++) {
		
		float r = (float) v_a[k].r;
		float g = (float) v_a[k].g;
		float b = (float) v_a[k].b;

		float r1 = (float) v_a[k+1].r;
		float g1 = (float) v_a[k+1].g;
		float b1 = (float) v_a[k+1].b;
		
		int x0 = (int)poly_r[k].x;
		int y0 = (int)poly_r[k].y;
		float z0 = (float) poly_r[k].z;
		int x1 = (int)poly_r[k+1].x; 
		int y1 = (int)poly_r[k+1].y;
		float z1 = (float) poly_r[k+1].z;


		int dx = abs(x1-x0);
		int dy = abs(y1-y0);

		int sx = (x0 < x1) ? 1 : -1;
		int sy = (y0 < y1) ? 1 : -1;
		
		int err = dx - dy;
		unsigned char c[4] = {wf[0],wf[1],wf[2],255};	
		int index = 0;
		while(true) {
			struct vector2D point = {x0,y0};
			curr_raster[index]=point;
			if ((x0==x1) && (y0==y1)) break;
			int e2 = 2 * err;
			if (e2 > -dy) { err -= dy; x0 += sx; }
			if (e2 < dx) { err += dx; y0 += sy; }
			index++;
		}
		
		int no_points = index;

		float delta_r = r1-r;
		float delta_g = g1-g;
		float delta_b = b1-b;
		float delta_z = z1-z0;

		for (int i = 0; i<no_points; i++) {
			float prop = (float) i/(no_points-1);
			int x = curr_raster[i].x;
			int y = curr_raster[i].y;
			
			struct Color col = {
				(unsigned char) (r + (prop*delta_r)),
				(unsigned char) (g + (prop*delta_g)),
				(unsigned char) (b + (prop*delta_b))
			};
			float depth = z0 + (prop*delta_z);
			struct edge_pixel n = {x,col,1/depth};
			edge_pixels[y-min][x-minx]=n;
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

void clear_edge_pixels() {
	
	for (int y = 0; y<WIN_HEIGHT; y++) {
		for (int x = 0; x<WIN_WIDTH; x++) {
			edge_pixels[y][x]=empty;
		}
	}
}

void draw_point(int x, int y, float z, int weight) {

	for (int i = x; i<weight+x; i++) {
		for (int j = y; j<weight+y; j++) {
			display->set_pixel(i-(weight/2),j-(weight/2),pc,z);
		}
	}	
	
}

void draw_line(float stx, float sty, float ex, float ey) {
			
		int x0 = (int) stx;
		int y0 = (int) sty;
		int x1 = (int) ex; 
		int y1 = (int) ey;


		int dx = abs(x1-x0);
		int dy = abs(y1-y0);

		int sx = (x0 < x1) ? 1 : -1;
		int sy = (y0 < y1) ? 1 : -1;
		
		int err = dx - dy;
		unsigned char c[4] = {wf2[0],wf2[1],wf2[2],255};	
		int index = 0;
		while(true) {
			display->set_pixel(x0,y0,c,-100);;
			if ((x0==x1) && (y0==y1)) break;
			int e2 = 2 * err;
			if (e2 > -dy) { err -= dy; x0 += sx; }
			if (e2 < dx) { err += dx; y0 += sy; }
			index++;
		}

}


struct vertex apply_transformation(struct vertex* v, float m[4][4]) {

	float x = ( v->x * m[0][0] ) + ( v->y * m[1][0] ) + ( v->z * m[2][0] ) + ( v->w * m[3][0] );
	float y = ( v->x * m[0][1] ) + ( v->y * m[1][1] ) + ( v->z * m[2][1] ) + ( v->w * m[3][1] );
	float z = ( v->x * m[0][2] ) + ( v->y * m[1][2] ) + ( v->z * m[2][2] ) + ( v->w * m[3][2] );
	float w = ( v->x * m[0][3] ) + ( v->y * m[1][3] ) + ( v->z * m[2][3] ) + ( v->w * m[3][3] );	

	struct vertex v2 = {x,y,z,w};
	return v2;

}

void draw_vector(Vec3 v1, Vec3 v2) {

	float x = (v1.x/v1.z)*WIN_WIDTH + (WIN_WIDTH/2);
	float y = (-v1.y/v1.z)*WIN_HEIGHT + (WIN_HEIGHT/2);
	
	float x1 = (v2.x/v2.z)*WIN_WIDTH + (WIN_WIDTH/2);
	float y1 = (-v2.y/v2.z)*WIN_HEIGHT + (WIN_HEIGHT/2);

	draw_line(x,y,x1,y1);	

}

void render_triangle(struct vertex* clip_coords[4], std::vector<struct vector3D>* colors) {

		
	//PERSPECTIVE DIVIDE HERE
	int max = 0;
	int maxx = 0;
	int min = 100000000;
	int minx = 100000000;
	struct vector3D poly_r[4];
	for (int k = 0; k<=3; k++) {

		struct vertex *v = clip_coords[k];
		float x = (v->x/v->w)*WIN_WIDTH + (WIN_WIDTH/2);
		float y = (-v->y/v->w)*WIN_HEIGHT + (WIN_HEIGHT/2);
		float z = v->w;
		struct vector3D r1 = {x,y,1/z};

		if (draw_vertex)
			draw_point(x,y,z,5);	

		poly_r[k]=r1;
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
	const int range = max-min+1;	
	const int rangex = maxx-minx+1;
	struct Color vertex_attributes[4] = {
			
		{colors->at(0).x,colors->at(0).y,colors->at(0).z},
		{colors->at(1).x,colors->at(1).y,colors->at(1).z},
		{colors->at(2).x,colors->at(2).y,colors->at(2).z},
		{colors->at(0).x,colors->at(0).y,colors->at(0).z},

	};

	get_pairs(poly_r,vertex_attributes,min,minx);
	for (int l = 0; l<range; l++) {
		int yval = l+min;
		int smallest = 100000000;
		int largest = -100000000;
		int is = 0;
		int il = 0;
		for (int b = 0; b<rangex; b++) {

			if (edge_pixels[l][b].x==-1)
				continue;
			if (edge_pixels[l][b].x <= smallest) {
				smallest=edge_pixels[l][b].x;
				is=b;
			}

			if (edge_pixels[l][b].x >= largest) {
				largest=edge_pixels[l][b].x;
				il=b;
			}
		}
		int first = smallest;
		int last = largest;
		
		//printf("first last - > (%d, %d)\n",first,last);	
		
		float startz = edge_pixels[l][is].depth;			
		float endz = edge_pixels[l][il].depth;			
			
		float startr = (float) edge_pixels[l][is].c.r;			
		float startg = (float) edge_pixels[l][is].c.g;			
		float startb = (float) edge_pixels[l][is].c.b;			
		
		float endr = (float) edge_pixels[l][il].c.r;
		float endg = (float) edge_pixels[l][il].c.g;
		float endb = (float) edge_pixels[l][il].c.b;
	
		float delta_z = endz-startz;
		float delta_r = endr-startr;
		float delta_g = endg-startg;
		float delta_b = endb-startb;
	
		//printf("first last - > (%d, %d)\n",first,last);
		//printf("newline\n");	
		for (int n=first; n<last+1; n++) {
			float prop = (float) (n-first)/(last-first+1);			
			float z = startz+ (prop*delta_z);
			if (edge_pixels[l][n-minx].x != -1) {
				//printf("depth %f %f\n",edge_pixels[l][n-minx].depth,z);
				if (!draw_wireframe) {
					unsigned char col[4] = {
						edge_pixels[l][n-minx].c.r,
						edge_pixels[l][n-minx].c.g,
						edge_pixels[l][n-minx].c.b,
						255
					};
					display->set_pixel(n,yval,col,z);
				} else {
					display->set_pixel(n,yval,wfc,z);
				}
				edge_pixels[l][n-minx]=empty;	
				continue;
			}

			unsigned char r = (unsigned char) (startr + (prop*delta_r));
			unsigned char g = (unsigned char) (startg + (prop*delta_g));
			unsigned char b = (unsigned char) (startb + (prop*delta_b));
			unsigned char color[4] = {
				r,g,b,255
			};
			display->set_pixel(n,yval,color,z);
		}
	}
}
void render_mesh(Model *m, Camera *camera) {

	//point_light[0][5]+=0.02f;
	//point_light[1][5]+=0.02f;
	unsigned char color[4] = {120,120,120,255};
	float sf = 0.75f;
	m->scale(sf,sf,sf);
	float tx = 0.0f;
	float ty = 0.0f;
	float tz = 0.0f;
	float tilt = 0.0f;	
	//m->rotate_y(1.5f);
	m->rotate_x(tilt);
	//m->rotate_y(1.5f);
	camera->lookAt(tx,ty,tz);
	m->translate(tx,ty,tz);
	float dir = 1.5f;
	float rr = 90;
	//camera->rotate_x(rr);
	camera->rotate_y(dir);
	//camera->zoom(0.01);
	camera->update_transform();
	
	std::vector<Vec3> all_lights;

	for (int i = 0; i<lights; i++) {	
	
		float lx = point_light[i][3];
		float ly = point_light[i][4];
		float lz = point_light[i][5];
		float lw = 1;

		struct vertex v0 = {lx,ly,lz,lw};
		struct vertex v1 = apply_transformation(&v0,camera->transform);		
		Vec3 light (v1.x,v1.y,v1.z);
		all_lights.push_back(light);

		struct vertex v2 = apply_transformation(&v1,projection_matrix);
		if (draw_lights) {
			//printf(" - %f %f\n",v2.x,v2.y);
			float _x = (v2.x/v2.w)*WIN_WIDTH + (WIN_WIDTH/2);
			float _y =  (-v2.y/v2.w)*WIN_HEIGHT + (WIN_HEIGHT/2);
			float _z = v2.w;
			//printf("%f %f draw\n",_x,_y);
			draw_point(_x,_y,_z,20);
		}

	}

	for (int i = 0; i<m->triangles(); i++) {
	
		struct mtl* mat = m->mats.at(i);
		bool has_normals = m->has_normals();
		//if (!(i==4 || i==5))
		//	continue;
	
		struct face f = m->faces.at(i);

		struct vertex *v00 = f.v0;
		struct vertex *v10 = f.v1;
		struct vertex *v20 = f.v2;

		struct vertex *n00 = f.n0;
		struct vertex *n10 = f.n1;
		struct vertex *n20 = f.n2;

		std::vector<struct vertex> normalv;;
		//n00->print();
		//n10->print();
		//n20->print();

		//world space	

		struct vertex v01 = apply_transformation(v00,camera->transform);
		struct vertex v11 = apply_transformation(v10,camera->transform);
		struct vertex v21 = apply_transformation(v20,camera->transform);

		if (has_normals && !flat_shade) {
			normalv.push_back(apply_transformation(n00,camera->transform));
			normalv.push_back(apply_transformation(n10,camera->transform));
			normalv.push_back(apply_transformation(n20,camera->transform));
		}

		//view space

		Vec3 vec0 (v01.x,v01.y,v01.z);
		Vec3 vec1 (v11.x,v11.y,v11.z);
		Vec3 vec2 (v21.x,v21.y,v21.z);
	
		Vec3 res1 = vec0.res(vec1);
		Vec3 res2 = vec0.res(vec2);
	
		Vec3 f_norm = res1.cross(res2).normalize();	
		//printf("norm -> ");
		//f_norm.print();	
		Vec3 vec4 (0,0,0);
		Vec3 diff = vec0;

		//backface culling

		if (backface_culling) {
			if (f_norm.dot(diff)>0) 
				continue;
		}

		//lighting	

		std::vector<Vec3> tvs = {vec0,vec1,vec2};
		std::vector<struct vector3D> colors;
	
		for (int j = 0; j<tvs.size(); j++) {
		
			float fill_r = 255.0f;
			float fill_g = 255.0f;
			float fill_b = 255.0f;
				
			float r = 0;
			float g = 0;
			float b = 0;
			float shininess = mat->Ns;		

			r+=(ambient_light[0]*mat->ka[0]);
			g+=(ambient_light[1]*mat->ka[1]);
			b+=(ambient_light[2]*mat->ka[2]);
	
			Vec3 norm;
			if (has_normals && !flat_shade)  {
				Vec3 new_n (normalv[j].x,normalv[j].y,normalv[j].z);
				norm = new_n;
			}
			else {
				norm = f_norm;
			}
			Vec3 ggnorm = norm.normalize();
			//printf("ggnorm -> ");
			//ggnorm.print();

		
			for (int k = 0; k<all_lights.size(); k++) {
				//diffuse light
				Vec3 curr_v = tvs[j];
				Vec3 light = all_lights[k];
				Vec3 to_light = curr_v.res(light);
				//draw_vector(light,mid2);
				//Vec3 normd = f_norm.mul(-0.1);
				//Vec3 nn = curr_v.add(normd);
				//draw_vector(mid2,nn);
				Vec3 l = to_light.normalize();	
		
				float h = std::max(l.dot(ggnorm),0.0f);
				if (h>0) {
					//printf("dot product = %f\n",l.dot(norm));
					//l.print();
					//norm.print();
				}
				else {
					//printf("\n");
				}
				//printf("%f dot product \n",h);

				r+=(h*mat->kd[0]*point_light[k][0]);
				g+=(h*mat->kd[1]*point_light[k][1]);
				b+=(h*mat->kd[2]*point_light[k][2]);
				
				//specular light
	
				Vec3 lr = l.mul(1);
				float h2 = lr.dot(ggnorm)*2;
				Vec3 ref1 (h2*ggnorm.x,h2*ggnorm.y,h2*ggnorm.z);
				Vec3 reflected = lr.res(ref1);
				Vec3 to_cam = curr_v.res(vec4).normalize();
					
				float rfdot = (float) to_cam.dot(reflected);
			
				float h0 = pow(std::max(rfdot,0.0f),shininess);	
				r+=(h0*mat->ks[0]*point_light[k][0]);
				g+=(h0*mat->ks[1]*point_light[k][1]);
				b+=(h0*mat->ks[2]*point_light[k][2]);
			
			}
		
			fill_r=std::min(r*255.0f,255.0f);
			fill_g=std::min(g*255.0f,255.0f);
			fill_b=std::min(b*255.0f,255.0f);
			
			struct vector3D col = {fill_r,fill_g,fill_b};
			colors.push_back(col);

				
		}
		//--------

		struct vertex v02 = apply_transformation(&v01,projection_matrix);
		struct vertex v12 = apply_transformation(&v11,projection_matrix);
		struct vertex v22 = apply_transformation(&v21,projection_matrix);
		struct vertex* clip_coords[4] = {&v02,&v12,&v22,&v02};
	
		//Normalized Device Coordinates	
		
		render_triangle(clip_coords,&colors);
	}
	//camera->rotate_x(-rr);
	//camera->rotate_y(-dir);
	m->translate(-tx,-ty,-tz);
	m->rotate_x(-tilt);
	//m->rotate_x(tilt);
	m->scale(1/sf,1/sf,1/sf);
}

void load_models(std::vector<std::string> paths) {


	for (int i = 0; i < paths.size(); i++) {

		std::string mesh_path = paths[i]+".obj";
		std::string mtl_path = paths[i]+".mtl";
		
		printf(">>%s\n",mesh_path.c_str());

		bool loaded;

		Mesh *mesh = new Mesh();
		loaded = mesh->load(mesh_path);
		mesh->display();

		if (loaded) {
			mesh->display();
		}
		else {
			std::cout << "Could not load: " << mesh_path << std::endl;
		}

		Material *material = new Material();
		loaded = material->load(mtl_path);	

		if (loaded) {
			material->display();
		}
		else {
			std::cout << "Could not load: " << mtl_path << std::endl;
		}

		material->display();	
		
		Model *model = new Model();
		model->apply_attr(mesh);
		model->apply_attr(material);
		
		models.push_back(model);	
	
		printf("model size %d\n",models.size());

	}
	
}

void render(Camera *camera) {
	for(int i = 0; i<models.size(); i++) {
		render_mesh(models[i],camera);
	}	
}

void update() {
	//TODO
}

int main(int argc, char* args[]) {

	
	display = new Display(WIN_WIDTH,WIN_HEIGHT,WINDOW_TITLE);
	display->init();
	display->set_clear_color(clear_color);
	clear_edge_pixels();
	Camera* camera = new Camera();
	camera->position(0,0.5,3);	
	camera->lookAt(0,0,0);
	int frames = 0;
	clock_t before = clock();

	std::vector<std::string> models;

	for (int i = 0; i < argc; i++) {
		std::vector<std::string> tmp;
		const std::string str(args[i]);
		std::cout << str << std::endl;
		_split(str,tmp,'.');
		if (tmp.size()>1) {
			std::cout << tmp[0] << std::endl;
			if (tmp[1]=="obj") {
				models.push_back(tmp[0]);
			}
		}
	}	

	load_models(models);
	while(1) {
		display->clear_buffer();
		if((clock() - before) / CLOCKS_PER_SEC > 1) {
			printf("fps %i\n",frames);
			before = clock();
			frames=0;
		}
		update();
		render(camera);
		display->flip_buffer();
		frames++;
	}
	display->destroy();

	return 0;
}


