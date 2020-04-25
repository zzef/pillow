#ifndef DISPLAY_H
#define DISPLAY_H
#include "SDL2/SDL.h"

struct pixel {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

class Display {

	public:

		unsigned char* buffer;
		float* depth_buffer;
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
