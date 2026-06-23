/*
 * stage.c: File with all the main game logic
 * Revision: 2026-06-24 fjluartes
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
static void doEnemies(void);
static void spawnEnemies(void);
static void clipPlayer(void);
static void drawFighters(void);

static Entity *player;
static SDL_Texture *enemyTexture;
static SDL_Texture *playerTexture;
static int enemySpawnTimer;
static int stageResetTimer;

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

    enemySpawnTimer = 0;

    stageResetTimer = FPS * 3;
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
    spawnEnemies();
    clipPlayer();
}

static void doPlayer(void)
{
    if (player != NULL)
    {
        player->dx = player->dy = 0;
        // if (player->reload > 0) player->reload--;
        if (app.keyboard[SDL_SCANCODE_UP]) player->dy = -PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_DOWN]) player->dy = PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_LEFT]) player->dx = -PLAYER_SPEED;
        if (app.keyboard[SDL_SCANCODE_RIGHT]) player->dx = PLAYER_SPEED;
        // fireBullet();
        // player->x += player->dx;
        // player->y += player->dy;
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

static void spawnEnemies(void)
{
    Entity *enemy;
    if (--enemySpawnTimer <= 0)
    {
        enemy = malloc(sizeof(Entity));
        memset(enemy, 0, sizeof(Entity));
        stage.fighterTail->next = enemy;
        stage.fighterTail = enemy;

        enemy->x = SCREEN_WIDTH;
        enemy->y = rand() % SCREEN_HEIGHT;
        enemy->texture = enemyTexture;
        SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));
        enemy->dy = -100 + (rand() % 200);
        enemy->dy /= 100;

        // enemy->side = SIDE_ALIEN;
        enemy->health = 1;

        // enemy->reload = FPS * (1 + (rand() % 3));

        enemySpawnTimer = 30 + (rand() % FPS);
    }
}

static void clipPlayer(void)
{
    if (player != NULL) 
    {
        if (player->x < 0) player->x = 0;
        if (player->y < 0) player->y = 0;
        if (player->x > SCREEN_WIDTH / 2) player->x = SCREEN_WIDTH / 2;
        if (player->y > SCREEN_HEIGHT) player->y = SCREEN_HEIGHT;
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