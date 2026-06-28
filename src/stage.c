/*
 * stage.c: File with all the main game logic
 * Revision: 2026-06-29 fjluartes
 */
#include "common.h"

#include "background.h"
#include "draw.h"
#include "sound.h"
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
static void doExplosions(void);
static void doDebris(void);
static void fireBullet(void);
static int bulletHitFighter(Entity *b);
static void spawnEnemies(void);
static void fireAlienBullet(Entity *e);
static void clipPlayer(void);
static void drawFighters(void);
static void drawBullets(void);
static void drawExplosions(void);
static void drawDebris(void);
static void addExplosions(int x, int y, int num);
static void addDebris(Entity *e);

static Entity *player;
static SDL_Texture *enemyTexture;
static SDL_Texture *playerTexture;
static SDL_Texture *bulletTexture;
static SDL_Texture *enemyBulletTexture;
static SDL_Texture *explosionTexture;
static int enemySpawnTimer;
static int stageResetTimer;

void initStage(void)
{
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail = &stage.debrisHead;

    enemyTexture = loadTexture("gfx/ufoRed.png");
    playerTexture = loadTexture("gfx/playerShip2_blue.png");
    bulletTexture = loadTexture("gfx/laserGreen10.png");
    enemyBulletTexture = loadTexture("gfx/laserRed02.png");
    explosionTexture = loadTexture("gfx/explosion.png");

    memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);

    resetStage();

    initPlayer();

    enemySpawnTimer = 0;

    stageResetTimer = FPS * 3;
}

void resetStage(void)
{
    Entity *e;
    Explosion *ex;
    Debris *d;

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

    while (stage.explosionHead.next)
    {
        ex = stage.explosionHead.next;
        stage.explosionHead.next = ex->next;
        free(ex);
    }

    while (stage.debrisHead.next)
    {
        d = stage.debrisHead.next;
        stage.debrisHead.next = d->next;
        free(d);
    }

    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;
    stage.explosionTail = &stage.explosionHead;
    stage.debrisTail = &stage.debrisHead;
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
    player->w /= 2;
    player->h /= 2;
    player->x = (SCREEN_WIDTH - player->w) / 2;
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
    doExplosions();
    doDebris();
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
            playSound(SND_PLAYER_FIRE, CH_PLAYER);
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
    bullet->side = SIDE_PLAYER;
    bullet->texture = bulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);
    bullet->w /= 2;
    bullet->h /= 2;
    bullet->x = player->x;
    bullet->y = player->y;
    bullet->dy = -PLAYER_BULLET_SPEED; // switch to bullet shooting up

    bullet->x += (player->w / 2) - (bullet->w / 2); // center bullet pos
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
                playSound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);
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

            if (e->x > SCREEN_WIDTH - e->w)
            {
                e->x = SCREEN_WIDTH - e->w;
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
    bullet->side = SIDE_ALIEN;
    bullet->texture = enemyBulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);
    bullet->w /= 2;
    bullet->h /= 2;

    bullet->x = e->x;
    bullet->y = e->y;

    bullet->x += (e->w -bullet->w) / 2;
    bullet->y += (e->h - bullet->h) / 2;

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
            collision(b->x, b->y, b->w, b->h, 
                      e->x, e->y, e->w, e->h))
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
            addExplosions(e->x + e->w / 2, e->y + e->h / 2, 32);
            addDebris(e);

            if (e == player)
            {
                playSound(SND_PLAYER_DIE, CH_PLAYER);
            }
            else
            {
                playSound(SND_ALIEN_DIE, CH_ANY);
            }
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
        enemy->w /= 2;
        enemy->h /= 2;
        // enemy starting pos at top of screen
        enemy->x = rand() % (SCREEN_WIDTH - enemy->w);
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
        if (player->x > SCREEN_WIDTH - player->w) 
            player->x = SCREEN_WIDTH - player->w;
        if (player->y > SCREEN_HEIGHT - player->h) 
            player->y = SCREEN_HEIGHT - player->h;
    }
}

static void doExplosions(void)
{
    Explosion *e, *prev;

    prev = &stage.explosionHead;

    for (e = stage.explosionHead.next; e != NULL; e = e->next)
    {
        e->x += e->dx;
        e->y += e->dy;

        if (--e->a <= 0)
        {
            if (e == stage.explosionTail) stage.explosionTail = prev;
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

static void doDebris(void)
{
    Debris *d, *prev;
    prev = &stage.debrisHead;

    for (d = stage.debrisHead.next; d != NULL; d = d->next)
    {
        d->x += d->dx;
        d->y += d->dy;

        d->dy += 0.5;

        if (--d->life <= 0)
        {
            if (d == stage.debrisTail) stage.debrisTail = prev;
            prev->next = d->next;
            d = prev;
        }
        else
        {
            prev = d;
        }
    }
}

static void addExplosions(int x, int y, int num)
{
    Explosion *e;
    int i;

    for (i = 0; i < num; i++)
    {
        e = malloc(sizeof(Explosion));
        memset(e, 0, sizeof(Explosion));
        stage.explosionTail->next = e;
        stage.explosionTail = e;

        e->x = x + (rand() % 32) - (rand() % 32);
        e->y = y + (rand() % 32) - (rand() % 32);
        e->dx = (rand() % 10) - (rand() % 9);
        e->dy = (rand() % 10) - (rand() % 9);
        e->dx /= 10;
        e->dy /= 10;
        

        switch (rand() % 4)
        {
            case 0:
                e->r = 255;
                break;
            case 1:
                e->r = 255;
                e->g = 128;
                break;
            case 2:
                e->r = 255;
                e->g = 255;
                break;
            default:
                e->r = 255;
                e->g = 255;
                e->b = 255;
                break;
        }
        e->a = 1 + rand() % (FPS * 3);
    }
}

static void addDebris(Entity *e)
{
    Debris *d;
    int x, y, w, h;

    w = e->w / 2;
    h = e->h / 2;

    for (y = 0; y <= h; y += h)
    {
        for (x = 0; x <= w; x += w)
        {
            d = malloc(sizeof(Debris));
            memset(d, 0, sizeof(Debris));
            stage.debrisTail->next = d;
            stage.debrisTail = d;

            d->x = e->x + e->w / 2;
            d->y = e->y + e->h / 2;
            d->dx = (rand() % 5) - (rand() % 5);
            d->dy = 5 + (rand() % 12);
            d->life = FPS * 2;
            d->texture = e->texture;

            d->rect.x = x;
            d->rect.y = y;
            d->rect.w = w;
            d->rect.h = h;
        }
    }
}

static void draw(void)
{
    drawBackground();
    drawStarfield();
    drawFighters();
    drawDebris();
    drawExplosions();
    drawBullets();
}

static void drawFighters(void)
{
    Entity *e;

    for (e = stage.fighterHead.next; e != NULL; e = e->next)
    {
        blit(e->texture, e->x, e->y, e->w, e->h);
    }
}

static void drawBullets(void)
{
    Entity *e;

    for (e = stage.bulletHead.next; e != NULL; e = e->next)
    {
        blit(e->texture, e->x, e->y, e->w, e->h);
    }
}

static void drawDebris(void)
{
    Debris *d;

    for (d = stage.debrisHead.next; d != NULL; d = d->next)
    {
        blitRect(d->texture, &d->rect, d->x, d->y);
    }
}

static void drawExplosions(void)
{
    Explosion *e;
    SDL_Rect dest;
    SDL_QueryTexture(explosionTexture, NULL, NULL, &dest.w, &dest.h);
    dest.w /= 2;
    dest.h /= 2;

    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(explosionTexture, SDL_BLENDMODE_ADD);

    for (e = stage.explosionHead.next; e != NULL; e = e->next)
    {
        SDL_SetTextureColorMod(explosionTexture, e->r, e->g, e->b);
        SDL_SetTextureAlphaMod(explosionTexture, e->a);

        blit(explosionTexture, e->x, e->y, dest.w, dest.h);
    }

    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
}