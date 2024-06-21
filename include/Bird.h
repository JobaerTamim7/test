#pragma once
#include"CommonFunction.h"
#include<vector>
#include<math.h>

class Bird
{
public:
	Bird(SDL_Window*,std::string);
	~Bird() {};
	void animate();
	void update();
	SDL_Rect get_bird_rect(){ return bird_rect; }
	SDL_Texture* get_dead_bird() { return bird_tex[4]; }
	bool fly = false;
	int life ;
private:
	SDL_Renderer* renderer;
	std::vector<SDL_Texture*> bird_tex;
	float frame;
	float vel_y;
	SDL_Rect bird_rect = { 50,WINDOW_HEIGHT / 2,45,40 };
};

