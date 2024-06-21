#include "Game.h"
#include<sstream>


Game::Game(SDL_Window* window,std::string bird_dir)
{
	win = window;
	renderer = SDL_GetRenderer(window);

	bird = new Bird(win,bird_dir);
	re = new Restart(win);
	texture_map["font_main_menu"] = common::load_font_texture("asset/font/minecraft.ttf", "Main Menu", renderer, BROWN, 500);
	texture_map["font_main_menu_active"] = common::load_font_texture("asset/font/minecraft.ttf", "Main Menu", renderer, GREEN, 500);
	texture_map["font_resume"] = common::load_font_texture("asset/font/minecraft.ttf", "Resume", renderer, BROWN, 500);
	texture_map["font_resume_active"] = common::load_font_texture("asset/font/minecraft.ttf", "Resume", renderer,GREEN, 500);
	texture_map["button_bg"] = common::load_texture("asset/texture/btn.png", renderer);
	texture_map["button_bg_active"] = common::load_texture("asset/texture/btn_active.png", renderer);
	texture_map["bg"] = common::load_texture("asset/texture/bg_game.png", renderer);
	texture_map["base"] = common::load_texture("asset/texture/base.png", renderer);
	texture_map["pause_bg"] = common::load_texture("asset/texture/pause_bg.png", renderer);
	texture_map["pause_font"] = common::load_font_texture("asset/font/minecraft.ttf", "Paused", renderer, BLACK, 500);
	texture_map["new"] = common::load_texture("asset/texture/new.png",renderer);

	chunck_map["flap"] = Mix_LoadWAV("asset/sound/flap.mp3");
	chunck_map["point"] = Mix_LoadWAV("asset/sound/point_gain.mp3");
	chunck_map["die"] = Mix_LoadWAV("asset/sound/die.mp3");

	game_music = Mix_LoadMUS("asset/sound/game_run.mp3");

	game_over = false;
	main_menu_state = false;
	pause = false;
	pause_done = false;
	quit_state = false;
	re_state = false;

	pause_interval = 0;
	score = 0;
	t = 3;

	dir_bird = bird_dir;
}

void Game::render()
{
	SDL_Rect bird_rect = bird->get_bird_rect();
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture_map["bg"], nullptr, &bg_rect);
	if (pause != true && !game_over)
	{
		if (Mix_PlayingMusic())
		{
			Mix_ResumeMusic();
		}
		else
		{
			Mix_PlayMusic(game_music, -1);
		}
		for (Pipe& pipe : pipes)
		{
			pipe.draw();
			pipe.update();
		}
		SDL_RenderCopy(renderer, texture_map["base"], nullptr, &base_rect);
		SDL_RenderCopy(renderer, texture_map["base"], nullptr, &base_rect2);
		update();
		bird->animate();
		bird->update();
		pipe_generate(win); 
		score_render();
	}

	else if (pause == true && game_over == false)
	{
		for (Pipe& pipe : pipes)
		{
			pipe.draw();
		}
		Game::pause_render();
	}

	else if (game_over == true)
	{
		Mix_HaltMusic();
		pause = false;
		texture_map["score"] = common::load_font_texture("asset/font/roboto.ttf", common::int_to_str(score), renderer, WHITE, 36);
		texture_map["h_score"] = common::load_font_texture("asset/font/roboto.ttf", common::int_to_str(h_score), renderer, WHITE, 36);
		for (Pipe& pipe : pipes)
		{
			pipe.draw();
		}

		SDL_RenderCopy(renderer, texture_map["base"], nullptr, &base_rect);
		SDL_RenderCopy(renderer, texture_map["base"], nullptr, &base_rect2);
		
		SDL_RenderCopyEx(renderer, bird->get_dead_bird(), nullptr, &bird_rect, 45, nullptr, SDL_FLIP_NONE);
		re->render();
		SDL_RenderCopy(renderer, texture_map["h_score"],nullptr, &over_h_score_rect);
		SDL_RenderCopy(renderer, texture_map["score"], nullptr, &over_score_rect);
		SDL_DestroyTexture(texture_map["score"]);
		SDL_DestroyTexture(texture_map["h_score"]);
		if (new_h_score == true)
		{
			SDL_RenderCopyEx(renderer, texture_map["new"], nullptr, &new_rect, 0, nullptr, SDL_FLIP_NONE);
		}
		//common::display_setter(renderer, texture_map["new"]);
	}
	
	//common::display_setter(renderer, texture_map["font_resume"]);
	SDL_RenderPresent(renderer);

	
}

void Game::handle_event()
{
	static bool fly_shut = false;
	static Uint32 pause_start;
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			quit_state = true;
			main_menu_state = true;
		}
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				pause = true;
				pause_start = SDL_GetTicks();
			}
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				if (!fly_shut)
				{
					bird->fly = true;
					fly_shut = true;
					Mix_PlayChannel(-1, chunck_map["flap"], 0);
				}
			}
		}
		if (event.type == SDL_KEYUP)
		{
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				fly_shut = false;
			}
		}
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (pause == true)
			{
				if (common::mouse_collision_rect(resume_button_rect) &&  game_over == false)
				{
					pause = false;
					pause_done = true;
					pause_interval = SDL_GetTicks() - pause_start;
				}
				if (common::mouse_collision_rect(main_menu_button_rect) &&  game_over == false)
				{
					game_over = true;
					main_menu_state = true;
				}
			}
			
			if (game_over == true)
			{
				if (common::mouse_collision_rect(re->get_menu_button()))
				{
					main_menu_state = true;
				}
				if (common::mouse_collision_rect(re->get_re_button()))
				{
					main_menu_state = true;
					game_over = false;
				}
			}
		}
		
	}
}

void Game::free_memory()
{
	for (auto& tex : texture_map) {
		if (tex.second) {
			SDL_DestroyTexture(tex.second);
		}
	}
	for (auto& chunck : chunck_map) {
		if (chunck.second) {
			Mix_FreeChunk(chunck.second);
		}
	}
	Mix_FreeMusic(game_music);
	pipes.clear();
	delete bird;
	delete re;

}

void Game::update()
{
	base_rect.x -= SCROLL_SPEED;
	base_rect2.x -= SCROLL_SPEED;

	if (base_rect.x + WINDOW_WIDTH < 0)
	{
		base_rect.x = -4 + WINDOW_WIDTH;
	}
	if (base_rect2.x + WINDOW_WIDTH < 0)
	{
		base_rect2.x = -4 + WINDOW_WIDTH;
	}
	if (is_collide() == true)
	{
		Mix_PlayChannel(-1, chunck_map["die"], 0);
		game_over = true;
		h_score = stoi(common::file_reader("Game_Text/high_score.txt"));
		if (h_score < score)
		{
			common::file_writer("Game_Text/high_score.txt", common::int_to_str(score));
			new_h_score = true;
		}
		h_score = stoi(common::file_reader("Game_Text/high_score.txt"));
	}

}

void Game::pipe_generate(SDL_Window* window)
{
	static Uint32 current_time = 0;
	Uint32 last_time = SDL_GetTicks();
	if (pause_done == true)
	{
		last_time -= pause_interval;
	}

	if (last_time - current_time  > 2000)
	{
		Pipe new_pipe(window);
		pipes.push_back(new_pipe);
		if (pause_done == true)
		{
			current_time = last_time + pause_interval;
		}
		else
		{
			current_time = last_time;
		}

		pause_done = false;
	}
	//std::cout << current_time << " " << last_time << std::endl;
	for (auto i = pipes.begin(); i != pipes.end();)
	{
		if (i->get_pipe_rect_up().x + i->get_pipe_rect_up().w < 0)
		{
			i = pipes.erase(i); 
		}
		else
		{
			i++; 
		}
	}
	
}

bool Game::is_collide()
{
	SDL_Rect bird_rect = bird->get_bird_rect();
	for (Pipe& pipe : pipes)
	{
		SDL_Rect pipe_rect_up = pipe.get_pipe_rect_up();
		SDL_Rect pipe_rect_down = pipe.get_pipe_rect_down();
		if (SDL_HasIntersection(&pipe_rect_up, &bird_rect) || SDL_HasIntersection(&pipe_rect_down,&bird_rect))
		{
			return true;
		}
	}
	if (bird_rect.y <= 0 || bird_rect.y + bird_rect.h >= 615)
	{
		return true;
	}
	return false;
}

void Game::score_render()
{
	static bool pipe_pass = false; 

	SDL_Rect bird_rect = bird->get_bird_rect();
	if (!pipes.empty())
	{
		SDL_Rect pipe_rect = pipes[0].get_pipe_rect_up(); 

		if (bird_rect.x > pipe_rect.x && bird_rect.x + bird_rect.w < pipe_rect.x + pipe_rect.w && !pipe_pass)
		{
			pipe_pass = true; 
		}

		if (pipe_pass && bird_rect.x + bird_rect.w > pipe_rect.x + pipe_rect.w)
		{
			Mix_PlayChannel(-1, chunck_map["point"], 0);
			score++; 
			pipe_pass = false; 
		}
	}
	std::string score_txt = common::int_to_str(score);
	SDL_Texture* score_tex = common::load_font_texture("asset/font/roboto.ttf", score_txt, renderer, WHITE, 36);
	SDL_RenderCopy(renderer, score_tex, nullptr, &score_rect);
	SDL_DestroyTexture(score_tex);
}

void Game::start_count()
{
	std::string t_txt = common::int_to_str(t);
	SDL_Texture* t_tex = common::load_font_texture("asset/font/minecraft.ttf", t_txt, renderer, BLACK, 90);
	SDL_Texture* bird_tex = common::load_texture(("asset/"+ dir_bird +"/frame-2.png").c_str(), renderer);
	SDL_Rect bird_rect = bird->get_bird_rect();
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture_map["bg"], nullptr, &bg_rect);
	SDL_RenderCopy(renderer, texture_map["base"], nullptr, &base_rect);
	SDL_RenderCopy(renderer, texture_map["base"], nullptr, &base_rect2);
	SDL_RenderCopy(renderer, bird_tex, nullptr, &bird_rect);
	SDL_RenderCopy(renderer, t_tex, nullptr, &t_rect);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(t_tex);
	SDL_DestroyTexture(bird_tex);
}

void Game::pause_render()
{
	SDL_RenderCopy(renderer, texture_map["pause_bg"], nullptr, &pause_bg_rect);
	SDL_RenderCopy(renderer, texture_map["base"], nullptr, &base_rect);
	SDL_RenderCopy(renderer, texture_map["base"], nullptr, &base_rect2);
	SDL_RenderCopy(renderer, texture_map["pause_font"], nullptr, &pause_rect);
	if (common::mouse_collision_rect(main_menu_button_rect))
	{
		SDL_RenderCopy(renderer, texture_map["button_bg_active"], nullptr, &main_menu_button_rect);
		SDL_RenderCopy(renderer, texture_map["font_main_menu_active"], nullptr, &main_menu_rect);
	}
	else
	{
		SDL_RenderCopy(renderer, texture_map["button_bg"], nullptr, &main_menu_button_rect);
		SDL_RenderCopy(renderer, texture_map["font_main_menu"], nullptr, &main_menu_rect);
	}
	if (common::mouse_collision_rect(resume_button_rect))
	{
		SDL_RenderCopy(renderer, texture_map["button_bg_active"], nullptr, &resume_button_rect);
		SDL_RenderCopy(renderer, texture_map["font_resume_active"], nullptr, &resume_rect);
	}
	else
	{
		SDL_RenderCopy(renderer, texture_map["button_bg"], nullptr, &resume_button_rect);
		SDL_RenderCopy(renderer, texture_map["font_resume"], nullptr, &resume_rect);
	}

}



