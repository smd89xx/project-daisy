#include "includes.h"

const u8 optX = 10;
const u8 optY = 8;
const u8 optNum = 9;
const u8 optXDelta = 13;
bool player = TRUE; // 0 - Lucy, 1 - Stephanie
u8 difficulty = 1; // 0 - Easy, 1 - Normal, 2 - Hard, 3 - Cement (name subject to change)
Sprite* confCurDiff;
Sprite* confCurChr;

const Option menuPrefs[] = 
{
    {optX+optXDelta, optY, "Easy"},
    {optX+optXDelta, optY+1, "Normal"},
    {optX+optXDelta, optY+2, "Hard"},
    {optX+optXDelta, optY+3, "Cement"},
    {optX+optXDelta, optY+5, "Lucy"},
    {optX+optXDelta, optY+6, "Stephanie"},
    {optX+optXDelta, optY+8, "00"},
    {optX+optXDelta, optY+9, "00"},
    {optX+(optXDelta >> 1) + 1, optY+12, "Exit"},
};

void selectOption_Prefs()
{
    SELECTION_SFX;
    u8 timer = palFadeTime;
    PAL_FADE_OUT;
    MUSIC_FADE;
    while(1)
    {
        timer--;
        SYS_doVBlankProcess();
        MDS_update();
        if (timer == 0)
        {
            SPR_releaseSprite(menuCursor);
            SPR_update();
            break;
        }
    }
    switch (*currentIndex)
    {
    case 8:
    {
        CLEAR_BG2;
        SPR_reset();
        MEM_free(currentIndex);
        MEM_free(musIndex);
        title();
        break;
    }
    default:
    {
        killExec(menuIndexInvalid);
        break;
    }
    }
}

static void joyEvent_Prefs(u16 joy, u16 changed, u16 state)
{
    if (joy != JOY_1)
    {
        return;
    }
    if (changed & state & BUTTON_UP)
    {
        curMove(menuPrefs,optNum,FALSE);
    }
    else if (changed & state & BUTTON_DOWN)
    {
        curMove(menuPrefs,optNum,TRUE);
    }
    if (changed & state & BUTTON_B)
    {
        *currentIndex = 8;
        selectOption_Prefs();
    }
    if (changed & state & BUTTON_START)
    {
        selectOption_Prefs();
    }
}

void preferences()
{
    u16 basetile = TILE_ATTR(PAL3,FALSE,FALSE,FALSE);
    CLEAR_BG1;
    VDP_drawText("Changes made will only take effect",2,0);
    VDP_drawText("upon starting a new game.",2,1);
    VDP_drawText("Difficulty:",optX,optY);
    VDP_drawText("Character:",optX,optY+5);
    VDP_drawText("Lives:",optX,optY+8);
    VDP_drawText("Sound Test:",optX,optY+9);
    drawMenu(menuPrefs,optNum,PAL3,BG_A);
    confCurDiff = SPR_addSprite(&heart,TILE_TO_PIXEL((menuPrefs[difficulty].x + 6)),TILE_TO_PIXEL(menuPrefs[difficulty].y),basetile);
    confCurDiff = SPR_addSprite(&heart,TILE_TO_PIXEL((menuPrefs[player].x + 9)),TILE_TO_PIXEL(menuPrefs[player + 4].y),basetile);
    JOY_setEventHandler(joyEvent_Prefs);
    fadePalette(titlePalette);
    MDS_request(MDS_BGM,BGM_MUS_S2BLVS);
    while (1)
    {
        *scroll -= *scrollSpeed;
        SYS_doVBlankProcess();
        SPR_update();
        MDS_update();
        VDP_setHorizontalScroll(BG_B,fix16ToInt(*scroll));
    }
}