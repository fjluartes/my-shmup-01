/*
 * Copyright (C) 2015-2018,2022 Parallel Realities. All rights reserved.
 */

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define STRNCPY(dest, src, n) \
    strncpy(dest, src, n);    \
    dest[n - 1] = '\0'

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

#define MAX_LINE_LENGTH 1024

#define FPS 60

#define PLAYER_SPEED 5
#define PLAYER_BULLET_SPEED 20
#define ALIEN_BULLET_SPEED 8

#define MAX_KEYBOARD_KEYS 350