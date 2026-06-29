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
    int i;

    memset(&highscores, 0, sizeof(Highscores));

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        highscores.highscore[i].score = NUM_HIGHSCORES - i;
        STRNCPY(highscores.highscore[i].name, "ANON", MAX_SCORE_NAME_LENGTH);
    }

    loadHighscores();
    newHighscore = NULL;
    cursorBlink = 0;
}

void initHighscores(void)
{
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);

    timeout = FPS * 5;
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
    doBackground();

    doStarfield();

    if (newHighscore != NULL)
    {
        doNameInput();
    }
    else
    {
        // initTitle();

        if (app.keyboard[SDL_SCANCODE_SPACE])
        {
            initStage();
        }
    }

    if (++cursorBlink >= FPS)
    {
        cursorBlink = 0;
    }
}

static void doNameInput(void)
{
    int i, n;
    char c;

    n = strlen(newHighscore->name);

    for (i = 0; i < (int)strlen(app.inputText); i++)
    {
        c = toupper(app.inputText[i]);
        if (n < MAX_SCORE_NAME_LENGTH - 1 && c >= ' ' && c <= 'Z')
        {
            newHighscore->name[n++] = c;
        }
    }

    if (n > 0 && app.keyboard[SDL_SCANCODE_BACKSPACE])
    {
        newHighscore->name[--n] = '\0';
        app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;
    }

    if (app.keyboard[SDL_SCANCODE_RETURN])
    {
        if (strlen(newHighscore->name) == 0)
        {
            STRNCPY(newHighscore->name, "ANON", MAX_SCORE_NAME_LENGTH);
        }
        newHighscore = NULL;
        saveHighscores();
    }
}

static void draw(void)
{
    drawBackground();

    drawStarfield();

    if (newHighscore != NULL)
    {
        drawNameInput();
    }
    else
    {
        drawHighscores();
        if (timeout % 40 < 20)
        {
            drawText(SCREEN_WIDTH / 2, 600, 255, 255, 255,
                TEXT_CENTER, "PRESS FIRE TO PLAY!");
        }
    }
}

static void drawNameInput(void)
{
    SDL_Rect r;

    drawText(SCREEN_WIDTH / 2, 70, 255, 255, 255, 
        TEXT_CENTER, "CONGRATS! HIGHSCORE!");
    drawText(SCREEN_WIDTH / 2, 120, 255, 255, 255, 
        TEXT_CENTER, "ENTER NAME BELOW:");
    drawText(SCREEN_WIDTH / 2, 250, 255, 255, 255, 
        TEXT_CENTER, newHighscore->name);

    if (cursorBlink < FPS / 2)
    {
        r.x = ((SCREEN_WIDTH / 4) + 
            (strlen(newHighscore->name) * GLYPH_WIDTH) / 2) + 5;
        r.y = 250;
        r.w = GLYPH_WIDTH;
        r.h = GLYPH_HEIGHT;

        SDL_SetRenderDrawColor(app.renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(app.renderer, &r);
    }

    drawText(SCREEN_WIDTH / 2, 625, 255, 255, 255,
        TEXT_CENTER, "PRESS RETURN WHEN FINISHED");
}

static void drawHighscores(void)
{
    int i, y, r, g, b;
    y = 150;

    drawText(SCREEN_WIDTH / 2, 70, 255, 255, 255, TEXT_CENTER, "HIGHSCORES");

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        r = 255;
        g = 255;
        b = 255;

        if (highscores.highscore[i].recent)
        {
            b = 0;
        }
        drawText(SCREEN_WIDTH / 2, y, r, g, b, TEXT_CENTER, "#%d. %-15s ... %03d", 
                (i + 1), highscores.highscore[i].name, highscores.highscore[i].score);

        y += 50;
    }
    drawText(SCREEN_WIDTH / 2, 600, 255, 255, 255, 
        TEXT_CENTER, "PRESS FIRE TO PLAY!");
}

void addHighscore(int score)
{
    Highscore newHighscores[NUM_HIGHSCORES + 1];
    int i;

    memset(newHighscores, 0, sizeof(Highscore) * (NUM_HIGHSCORES + 1));

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        newHighscores[i] = highscores.highscore[i];
        newHighscores[i].recent = 0;
    }

    newHighscores[NUM_HIGHSCORES].score = score;
    newHighscores[NUM_HIGHSCORES].recent = 1;

    qsort(newHighscores, NUM_HIGHSCORES + 1, sizeof(Highscore), highscoreComparator);

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        highscores.highscore[i] = newHighscores[i];

        if (highscores.highscore[i].recent)
            newHighscore = &highscores.highscore[i];
    }
}

static int highscoreComparator(const void *a, const void *b)
{
    Highscore *h1 = ((Highscore *)a);
    Highscore *h2 = ((Highscore *)b);

    return h2->score - h1->score;
}