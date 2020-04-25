#ifndef DISPLAY_H
#define DISPLAY_H

#include "SDL2/SDL.h"

#define MAX_W 1920
#define MAX_H 1080
#define MIN_W 640
#define MIN_H 480

struct pixel {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

class Display {

	public:

		unsigned char buffer[MAX_W][MAX_H][4];
		float depth_buffer[MAX_W][MAX_H];
		int height;
		int width;	
		std::string title;
		unsigned char clc[4] = {30,30,30,255};
		SDL_Texture *Frame;
		SDL_Window *window;
		SDL_Renderer *renderer;
	
		Display(int width, int height, std::string title);
		void clear_buffer();
		void init();
		void flip_buffer();
		void set_pixel(int x, int y, unsigned char* color, float depth);
		void destroy();

	private:
		void prepare_buffers();
};


#endif
