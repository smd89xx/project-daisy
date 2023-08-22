#pragma once

#include "includes.h"

enum consoleRegions {ntscJPN = 0x20, ntscUSA = 0xA0, palEUR = 0xE0, palJPN = 0x60};

typedef struct
{
    u8 x;
    u8 y;
    char* label;
}Option;

extern u8 palFadeTime;
extern u8 vblankRate;
extern const u8* region;
extern u8* currentIndex;
extern u8* musIndex;
extern fix16* scroll;
extern fix16* scrollSpeed;
extern Sprite* menuCursor;

#define MUSIC_FADE          MDS_fade(127,7,TRUE)
#define PAL_FADE_OUT        PAL_fadeOutAll(palFadeTime,TRUE);
#define CLEAR_BG1           VDP_clearPlane(BG_A,TRUE);
#define CLEAR_BG2           VDP_clearPlane(BG_B,TRUE);
#define TILE_TO_PIXEL(tile) (tile << 3)
#define SELECTION_SFX       MDS_request(MDS_SE1,BGM_SFX_S1SELECT)
#define SIZEOF_8BIT         1
#define SIZEOF_16BIT        2
#define SIZEOF_32BIT        4

void fadePalette(const u16* palette);
void drawMenu(const Option* option, u8 length, u8 paletteLine, u8 plane);
void curMove(const Option* option, u8 length, bool direction);
void title();