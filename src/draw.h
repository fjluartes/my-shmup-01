/*
 * Copyright (C) 2015-2018,2022 Parallel Realities. All rights reserved.
 */

void prepareScene(void);
void presentScene(void);
SDL_Texture *loadTexture(char *filename);
void blit(SDL_Texture *texture, int x, int y, int w, int h);
