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

#define MUSIC_FADE          MDS_fade(127,7,TRUE)
#define PAL_FADE_OUT        PAL_fadeOutAll(palFadeTime,TRUE);
#define CLEAR_BG1           VDP_clearPlane(BG_A,TRUE);
#define CLEAR_BG2           VDP_clearPlane(BG_B,TRUE);
#define TILE_TO_PIXEL(tile) (tile << 3)
#define SELECTION_SFX       MDS_request(MDS_SE1,BGM_SFX_S1SELECT)