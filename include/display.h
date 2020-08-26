#ifndef DISPLAY_H
#define DISPLAY_H

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <queue>

#define MAX_W 1920
#define MAX_H 1080
#define MIN_W 640
#define MIN_H 480

struct s_text{
	std::string text;
	char color[3];
	int x;	
	int y;
	int size;
};


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
		int FONT_H = 30;	
		std::string title;
		unsigned char clc[4] = {30,30,30,255};
		SDL_Texture *Frame;
		SDL_Window *window;
		SDL_Renderer *renderer;
		TTF_Font *font;	
		bool depth_buffering = true;

		Display(int width, int height, std::string title);
		void show();
		void clear_buffer();
		void init();
		void draw_text(const std::string& str, int x, int y, char* color, int size);
		void draw_text(std::string&& str, int x, int y, char* color, int size);
		void flip_buffer();
		void set_pixel(int x, int y, unsigned char* color, float depth);
		void destroy();
		void set_clear_color(unsigned char color[4]);
		void toggle_depth_buffer();

	private:
		void prepare_buffers();
};


#endif
