#include "SDL2/SDL.h"
#include "../include/includes.h"
#include "../include/display.h"

Display::Display(int width, int height, std::string title) {
	this->width=std::max(MIN_W,std::min(width,MAX_W));
	this->height=std::max(MIN_H,std::min(height,MAX_H));	
	this->title=title;
	
}


void Display::set_clear_color(unsigned char color[4]) {
	this->clc[0]=color[0];
	this->clc[1]=color[1];
	this->clc[2]=color[2];
	this->clc[3]=color[3];
}

void Display::clear_buffer() {
	for (int i = 0; i<this->width; i++) {
		for (int j = 0; j<this->height; j++) {
			this->buffer[i][j][0]=this->clc[3];	
			this->buffer[i][j][1]=this->clc[2];
			this->buffer[i][j][2]=this->clc[1];
			this->buffer[i][j][3]=this->clc[0];
			this->depth_buffer[i][j]=10000000000;	
		}
	}

}

void Display::init() {
	//Start SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//We start by creating a window
	this->window = SDL_CreateWindow(
		this->title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		this->width,
		this->height,
		SDL_WINDOW_SHOWN
	);
	//We need a renderer to do our rendering
	this->renderer = SDL_CreateRenderer(this->window,-1,SDL_RENDERER_ACCELERATED);
	//Create frame
	this->Frame = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING, this->width, this->height
	);

}	

void Display::flip_buffer() {
	unsigned char* bytes = nullptr;
	int pitch = 0;
	//get locked pixels and store in bytes for write access
	SDL_LockTexture(this->Frame,nullptr,(void**)(&bytes),&pitch);
	for (int y = 0; y < this->height; y++) {
		for (int x = 0; x < this->width; x++) {
			memcpy(&bytes[(y*this->width+x)*4],this->buffer[x][y],4);
		}
	}
	SDL_UnlockTexture(this->Frame);
	SDL_RenderCopy(renderer,Frame,NULL,NULL);		
	//show
	SDL_RenderPresent(this->renderer);
}

void Display::set_pixel(int x, int y, unsigned char* color,float depth) {

	if (x<0 || x >= this->width) {
		//////printf("clipped x\n");
		return;
	}
	if (y<0 || y >= this->height) {
		//////printf("clipped y\n");
		return;
	}

	float d = this->depth_buffer[x][y];
	if (depth<d) {
		this->depth_buffer[x][y]=depth;
	}
	else {
		return;
	}

	this->buffer[x][y][0]=color[3];
	this->buffer[x][y][1]=color[2];
	this->buffer[x][y][2]=color[1];
	this->buffer[x][y][3]=color[0];
}

void Display::destroy() {
	SDL_DestroyWindow(this->window);
	SDL_Quit();
}

