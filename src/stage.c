/*
 * stage.c: File with all the main game logic
 */
#include "common.h"

#include "draw.h"

extern App app;
extern Stage stage;

static void logic(void);
static void draw(void);
static void doPlayer(void);
static void drawPlayer(void);

void initStage(void)
{
    app.delegate.logic = logic;
    app.delegate.draw = draw;
}

static void logic(void)
{

}

static void doPlayer(void)
{

}

static void draw(void)
{

}

static void drawPlayer(void)
{
    
}