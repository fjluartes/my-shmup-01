/*
 * stage.c: File with all the main game logic
 * Revision: 2026-06-25 fjluartes
 */
#include "common.h"

#include "background.h"
#include "draw.h"
#include "stage.h"
#include "util.h"

extern App app;
extern Stage stage;

static void logic(void);
static void draw(void);
static void initPlayer(void);
static void resetStage(void);
static void doPlayer(void);
static void doFighters(void);
static void doEnemies(void);
static void doBullets(void);
static void fireBullet(void);
static int bulletHitFighter(Entity *b);
static void spawnEnemies(void);
static void fireAlienBullet(Entity *e);
static void clipPlayer(void);
static void drawFighters(void);
static void drawBullets(void);

static Entity *player;
static SDL_Texture *enemyTexture;
static SDL_Texture *playerTexture;
static SDL_Texture *bulletTexture;
static SDL_Texture *enemyBulletTexture;
static int enemySpawnTimer;
static int stageResetTimer;

void initStage(void)
{
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;

    enemyTexture = loadTexture("gfx/ufoRed.png");
    playerTexture = loadTexture("gfx/playerShip2_blue.png");
    bulletTexture = loadTexture("gfx/laserGreen10.png");
    enemyBulletTexture = loadTexture("gfx/laserRed02.png");

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

    while (stage.bulletHead.next)
    {
        e = stage.bulletHead.next;
        stage.bulletHead.next = e->next;
        free(e);
    }

    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;
}

static void initPlayer(void)
{
    player = malloc(sizeof(Entity));
    memset(player, 0, sizeof(Entity));
    stage.fighterTail->next = player;
    stage.fighterTail = player;

    player->health = 3;
    player->texture = playerTexture;
    SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
    player->x = (SCREEN_WIDTH / 2) - (player->w / 4);
    player->y = (SCREEN_HEIGHT * 3) / 4;

    player->side = SIDE_PLAYER;
}

static void logic(void)
{
    doBackground();
    doStarfield();
    doPlayer();
    doEnemies();
    doFighters();
    doBullets();
    spawnEnemies();
    clipPlayer();
    if (player == NULL && --stageResetTimer <= 0)
    {
        initStage(); // replace by initHighscores
    }
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
        if (app.keyboard[SDL_SCANCODE_SPACE] && player->reload == 0)
        {
            fireBullet();
        }
    }
}

static void fireBullet(void)
{
    Entity *bullet;
    bullet = malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->health = 1;
    bullet->x = player->x;
    bullet->y = player->y;
    bullet->dy = -PLAYER_BULLET_SPEED; // switch to bullet shooting up
    bullet->texture = bulletTexture;
    bullet->side = SIDE_PLAYER;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->x += (player->w / 4) - (bullet->w / 4); // center bullet pos
    player->reload = 8;
}

static void doEnemies(void)
{
    Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next)
    {
        if (e != player)
        {
            if (player != NULL && --e->reload <= 0)
            {
                fireAlienBullet(e);
            }
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

        // clip enemy fighter & reverse dx on walls
        if (e != player)
        {
            if (e->x < 0)
            {
                e->x = 0;
                e->dx = -e->dx;
            }

            if (e->x > SCREEN_WIDTH - (e->w / 2))
            {
                e->x = SCREEN_WIDTH - (e->w / 2);
                e->dx = -e->dx;
            }
        }

        if (e != player && e->y > SCREEN_HEIGHT)
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
        else
        {
            prev = e;
        }
    }
}

static void doBullets(void)
{
    Entity *b, *prev;

    prev = &stage.bulletHead;

    for (b = stage.bulletHead.next; b != NULL; b = b->next)
    {
        b->x += b->dx;
        b->y += b->dy;

        if (bulletHitFighter(b) || b->x < -b->w || b->y < -b->h ||
            b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT)
        {
            if (b == stage.bulletTail) stage.bulletTail = prev;
            prev->next = b->next;
            free(b);
            b = prev;
        }
        else 
        {
            prev = b;
        }
    }
}

static void fireAlienBullet(Entity *e)
{
    Entity *bullet;
    bullet = malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->health = 1;
    bullet->x = e->x;
    bullet->y = e->y;
    bullet->texture = enemyBulletTexture;
    bullet->side = SIDE_ALIEN;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->x += (e->w / 2) - (bullet->w / 2);
    bullet->y += (e->h / 2) - (bullet->h / 2);

    calcSlope(player->x + (player->w / 2),
              player->y + (player->h / 2),
              bullet->x, bullet->y,
              &bullet->dx, &bullet->dy);

    bullet->dx *= ALIEN_BULLET_SPEED;
    bullet->dy *= ALIEN_BULLET_SPEED;
    e->reload = FPS + (rand() % (FPS * 2));
}

static int bulletHitFighter(Entity *b)
{
    Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next)
    {
        if (e->side != b->side &&
            collision(b->x, b->y, b->w / 2, b->h / 2, 
                      e->x, e->y, e->w / 2, e->h / 2))
        {
            if (e == player && e->health > 0)
            {
                e->health -= 1;
                b->health = 0;
            }
            else
            {
                e->health = 0;
                b->health = 0;
            }
            // Play die sounds
            return 1;
        }
    }
    return 0;
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

        enemy->texture = enemyTexture;
        SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);
        // enemy starting pos at top of screen
        enemy->x = rand() % (SCREEN_WIDTH - enemy->w / 2);
        enemy->y = -enemy->h;

        enemy->dx = 0;
        enemy->dy = 2 + (rand() % 3); // move enemy fighters downward

        enemy->side = SIDE_ALIEN;
        enemy->health = 1;

        enemy->reload = FPS * (1 + (rand() % 3));

        enemySpawnTimer = 30 + (rand() % FPS);
    }
}

static void clipPlayer(void)
{
    if (player != NULL) 
    {
        if (player->x < 0) player->x = 0;
        if (player->y < 0) player->y = 0;
        if (player->x > SCREEN_WIDTH - (player->w / 2)) 
            player->x = SCREEN_WIDTH - (player->w / 2);
        if (player->y > SCREEN_HEIGHT - (player->h / 2)) 
            player->y = SCREEN_HEIGHT - (player->h / 2);
    }
}

static void draw(void)
{
    drawBackground();
    drawStarfield();
    drawFighters();
    drawBullets();
}

static void drawFighters(void)
{
    Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next)
    {
        blit(e->texture, e->x, e->y, e->w / 2, e->h / 2);
    }
}

static void drawBullets(void)
{
    Entity *e;

    for (e = stage.bulletHead.next; e != NULL; e = e->next)
    {
        blit(e->texture, e->x, e->y, e->w / 2, e->h / 2);
    }
}