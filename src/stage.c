/*
 * stage.c: File with all the main game logic
 * Revision: 2026-06-23 fjluartes
 */
#include "common.h"

#include "draw.h"
#include "stage.h"

extern App app;
extern Stage stage;

static void logic(void);
static void draw(void);
static void initPlayer(void);
static void resetStage(void);
static void doPlayer(void);
static void doFighters(void);
static void drawFighters(void);

static Entity *player;
static SDL_Texture *enemyTexture;
static SDL_Texture *playerTexture;

void initStage(void)
{
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;

    enemyTexture = loadTexture("gfx/enemy.png");
    playerTexture = loadTexture("gfx/player.png");

    memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);

    resetStage();

    initPlayer();
}

void resetStage(void)
{
    Entity *e;

    while (stage.fighterHead.next)
    {
        e = stage.fighterHead.next;
        stage.fighterHead.next = e->next;
        free(e);
    }

    stage.fighterTail = &stage.fighterHead;
}

static void initPlayer(void)
{
    player = malloc(sizeof(Entity));
    memset(player, 0, sizeof(Entity));
    stage.fighterTail->next = player;
    stage.fighterTail = player;

    player->health = 3;
    player->x = 100;
    player->y = 100;
    player->texture = playerTexture;
    SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
}

static void logic(void)
{
    doPlayer();
    doEnemies();
    doFighters();
}

static void doPlayer(void)
{
    if (player != NULL)
    {
        player->dx = player->dy = 0;
        if (player->reload > 0) player->reload--;
        if (app.keyboard[SDL_SCANCODE_UP]) player->dy = -PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_DOWN]) player->dy = PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_LEFT]) player->dx = -PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_RIGHT]) player->dx = PLAYER_SPEED;
        // fireBullet();
    }
}

static void doEnemies(void)
{
    Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next)
    {
        if (e != player)
        {
            e->y = MIN(MAX(e->y, 0), SCREEN_HEIGHT - e->h);

            // if (player != NULL && --e->reload <= 0)
            // {
            //     fireAlienBullet(e);
            //     playSound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);
            // }
        }
    }
}

static void doFighters(void)
{
    Entity *e, *prev;

    prev = &stage.fighterHead;

    for (e = stage.fighterHead.next; e != NULL; e = e->next)
    {
        e->x += e->dx;
        e->y += e->dy;

        if (e != player && e->x < -e->w)
        {
            e->health = 0;
        }
        if (e->health == 0)
        {
            if (e == player) player = NULL;
            if (e == stage.fighterTail) stage.fighterTail = prev;
            prev->next = e->next;
            free(e);
            e = prev;
        }
        prev = e;
    }
}

static void draw(void)
{
    drawFighters();
}

static void drawFighters(void)
{
    Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next)
    {
        blit(e->texture, e->x, e->y);
    }
}