/*
 * highscores.c: File to save and show highscores
 */
#include "common.h"

#include "background.h"
#include "highscores.h"
#include "stage.h"
#include "text.h"

extern App app;
extern Highscores highscores;

static void logic(void);
static void draw(void);
static int highscoreComparator(const void *a, const void *b);
static void drawHighscores(void);
static void doNameInput(void);
static void drawNameInput(void);
// Save and load highscores
static void saveHighscores(void);
static void loadHighscores(void);

static Highscore *newHighscore;
static int cursorBlink;
static int timeout;

void initHighscoreTable(void)
{

}

void initHighscores(void)
{

}

static void saveHighscores(void)
{
    FILE *fp;
    int i;

    fp = fopen("src/highscores.dat", "wb");
    if (fp == NULL)
    {
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
            "Failed to open highscores.dat for writing");
        return;
    }

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        fwrite(highscores.highscore[i].name, sizeof(char), MAX_SCORE_NAME_LENGTH, fp);
        fwrite(&highscores.highscore[i].score, sizeof(int), 1, fp);
    }

    fclose(fp);
}

static void loadHighscores(void)
{
    FILE *fp;
    int i;

    fp = fopen("src/highscores.dat", "rb");
    if (fp == NULL)
    {
        // No file yet
        return;
    }

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        fread(highscores.highscore[i].name, sizeof(char), MAX_SCORE_NAME_LENGTH, fp);
        fread(&highscores.highscore[i].score, sizeof(int), 1, fp);
        highscores.highscore[i].recent = 0;
    }

    fclose(fp);
}

static void logic(void)
{

}

static void doNameInput(void)
{

}

static void draw(void)
{

}

static void drawNameInput(void)
{

}

static void drawHighscores(void)
{

}

void addHighscore(int score)
{

}

static int highscoreComparator(const void *a, cosnt void *b)
{
    Highscore *h1 = ((Highscore *)a);
    Highscore *h2 = ((Highscore *)b);

    return h2->score = h1->score;
}