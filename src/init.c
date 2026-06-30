/*
 * Copyright (C) 2015-2018,2022 Parallel Realities. All rights reserved.
 */
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "common.h"

#include "background.h"
#include "highscores.h"
#include "init.h"
#include "sound.h"
#include "text.h"
#include "stage.h"

extern App app;

void initSDL(void)
{
	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;

	windowFlags = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("Couldn't initializ SDL Mixer\n");
		exit(1);
	}

	Mix_AllocateChannels(MAX_SND_CHANNELS);

	app.window = SDL_CreateWindow("MY SHMUP 01", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	if (!app.window)
	{
		printf("Failed to open %d x %d window: %s\n", 
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	if (!app.renderer)
	{
		printf("Failed to create renderer: %s\n", SDL_GetError());
		exit(1);
	}

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	SDL_ShowCursor(0);
}

void initGame(void)
{
	initBackground();

	initStarfield();

	initSounds();

	initFonts();

	initHighscoreTable();

	loadMusic("music/Mercury.ogg");

	playMusic(1);
}

void cleanup(void)
{
	Mix_Quit();

	IMG_Quit();

	SDL_DestroyRenderer(app.renderer);

	SDL_DestroyWindow(app.window);

	SDL_Quit();
}
