/*
 * background.c: File to draw and render background (vertical scroll)
 */
#include "common.h"

#include "background.h"
#include "draw.h"

extern App app;

static int backgroundY;
static Star stars[MAX_STARS];
static SDL_Texture *background;

void initBackground(void)
{
    background = loadTexture("gfx/background.png");
    backgroundY = 0;
}

void initStarfield(void)
{
    int i;

    for (i = 0; i < MAX_STARS; i++)
    {
        stars[i].x = rand() % SCREEN_WIDTH;
        stars[i].y = rand() % SCREEN_HEIGHT;
        stars[i].speed = 1 + rand() % 8;
    }
}

void doBackground(void)
{
    if (++backgroundY > SCREEN_HEIGHT)
        backgroundY = 0;
}

void doStarfield(void)
{
    int i;

    for (i = 0; i < MAX_STARS; i++)
    {
        stars[i].y += stars[i].speed;
        if (stars[i].y >= SCREEN_HEIGHT)
        {
            stars[i].y -= SCREEN_HEIGHT;
        }
    }
}

void drawStarfield(void)
{
    int i, c;

    for (i = 0; i < MAX_STARS; i++)
    {
        c = 32 * stars[i].speed;
        SDL_SetRenderDrawColor(app.renderer, c, c, c, 255);
        SDL_RenderDrawLine(app.renderer, 
                           stars[i].x, 
                           stars[i].y, 
                           stars[i].x, 
                           stars[i].y + 3);
    }
}

void drawBackground(void)
{
    SDL_Rect dest;
    int y;

    for (y = backgroundY - SCREEN_HEIGHT; y < SCREEN_HEIGHT; y += SCREEN_HEIGHT)
    {
        dest.x = 0;
        dest.y = y;
        dest.w = SCREEN_WIDTH;
        dest.h = SCREEN_HEIGHT;
        SDL_RenderCopy(app.renderer, background, NULL, &dest);
    }
}