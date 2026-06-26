/*
 * Copyright (C) 2015-2018,2022 Parallel Realities. All rights reserved.
 */
typedef struct Entity Entity;
typedef struct Explosion Explosion;

typedef struct 
{
	void (*logic)(void);
	void (*draw)(void);
} Delegate;

typedef struct
{
	SDL_Renderer *renderer;
	SDL_Window   *window;
	Delegate delegate;
	int keyboard[MAX_KEYBOARD_KEYS];
	char inputText[MAX_LINE_LENGTH];
} App;

struct Entity
{
	float x;
	float y;
	int w;
	int h;
	float dx;
	float dy;
	int health;
	int reload;
	int side;
	SDL_Texture *texture;
	Entity *next;
};

struct Explosion
{
	float x;
	float y;
	float dx;
	float dy;
	int w;
	int h;
	int r, g, b, a;
	Explosion *next;
};

typedef struct
{
	Entity fighterHead, *fighterTail;
	Entity bulletHead, *bulletTail;
	Explosion explosionHead, *explosionTail;
	int score;
} Stage;

typedef struct
{
	int x;
	int y;
	int speed;
} Star;
