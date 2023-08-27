#include "includes.h"

const u8 optX = 10;
const u8 optY = 8;
const u8 optNum = 9;
const u8 optXDelta = 13;
const u8 sfxStart = 4;
const u8 livesInd = 6;
const u8 sndInd = 7;
bool player = TRUE; // 0 - Lucy, 1 - Stephanie
u8 difficulty = 1; // 0 - Easy, 1 - Normal, 2 - Hard, 3 - Hardest (Names subject to change) 
Sprite* confCurDiff;
Sprite* confCurChr;
u8 lives = 5;

const Option menuPrefs[] = 
{
    {optX+optXDelta, optY, "Sissy"},
    {optX+optXDelta, optY+1, "Average"},
    {optX+optXDelta, optY+2, "Honors"},
    {optX+optXDelta, optY+3, "Hell^"},
    {optX+optXDelta, optY+5, "Lucy"},
    {optX+optXDelta, optY+6, "Stephanie"},
    {optX+optXDelta, optY+8, "05"},
    {optX+optXDelta, optY+9, "00"},
    {optX+(optXDelta >> 1) + 1, optY+12, "Exit"},
};

static void selectorMove(bool type)
{
    u8 szStr = strlen(menuPrefs[*currentIndex].label);
    if (!type)
    {
        SPR_setPosition(confCurDiff,TILE_TO_PIXEL(menuPrefs[*currentIndex].x + szStr),TILE_TO_PIXEL(menuPrefs[*currentIndex].y));
        difficulty = *currentIndex;
    }
    else
    {
        SPR_setPosition(confCurChr,TILE_TO_PIXEL(menuPrefs[*currentIndex].x + szStr),TILE_TO_PIXEL(menuPrefs[*currentIndex].y));
        player = *currentIndex - 4;
    }
}

static void selectOption_Prefs()
{
    SELECTION_SFX;
    switch (*currentIndex)
    {
    case 6:
    {
        break;
    }
    case 7:
    {
        if (*musIndex < sfxStart)
        {
            MDS_request(MDS_BGM,*musIndex);
        }
        else
        {
            MDS_request(MDS_SE1,*musIndex);
        }
        break;
    }
    case 8:
    {
        JOY_setEventHandler(NULL);
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
        CLEAR_BG2;
        SPR_reset();
        MEM_free(currentIndex);
        currentIndex = NULL;
        MEM_free(musIndex);
        musIndex = NULL;
        title();
        break;
    }
    default:
    {
        if (*currentIndex < 4)
        {
            selectorMove(FALSE);
            break;
        }
        else if (*currentIndex < 6)
        {
            selectorMove(TRUE);
            break;
        }
        JOY_setEventHandler(NULL);
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
        killExec(menuIndexInvalid);
        break;
    }
    }
}

static void drawMenuHex(bool direction, bool type, bool accel)
{
    SELECTION_SFX;
    char hexStr[3] = "00";
    if (!direction)
    {
        if (!type)
        {
            if (!accel)
            {
                lives--;
            }
            else
            {
                lives -= 0x10;
            }
            intToHex(lives,hexStr,2);
        }
        else
        {
            if (!accel)
            {
                *musIndex -= 1;
            }
            else
            {
                *musIndex -= 0x10;
            }
            intToHex(*musIndex,hexStr,2);
        }
    }
    else
    {
        if (!type)
        {
            if (!accel)
            {
                lives++;
            }
            else
            {
                lives += 0x10;
            }
            intToHex(lives,hexStr,2);
        }
        else
        {
            if (!accel)
            {
                *musIndex += 1;
            }
            else
            {
                *musIndex += 0x10;
            }
            intToHex(*musIndex,hexStr,2);
        }
    }
    VDP_drawText(hexStr,menuPrefs[*currentIndex].x,menuPrefs[*currentIndex].y);
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
    if (changed & state & BUTTON_LEFT)
    {
        if (*currentIndex == livesInd)
        {
            drawMenuHex(FALSE,FALSE,FALSE);
        }
        else if (*currentIndex == sndInd)
        {
            drawMenuHex(FALSE,TRUE,FALSE);
        }
    }
    else if (changed & state & BUTTON_RIGHT)
    {
        if (*currentIndex == livesInd)
        {
            drawMenuHex(TRUE,FALSE,FALSE);
        }
        else if (*currentIndex == sndInd)
        {
            drawMenuHex(TRUE,TRUE,FALSE);
        }
    }
    if (changed & state & BUTTON_B)
    {
        *currentIndex = 8;
        selectOption_Prefs();
    }
    else if (changed & state & BUTTON_A)
    {
        if (*currentIndex == livesInd)
        {
            drawMenuHex(FALSE,FALSE,TRUE);
        }
        else if (*currentIndex == sndInd)
        {
            drawMenuHex(FALSE,TRUE,TRUE);
        }
    }
    else if (changed & state & BUTTON_C)
    {
        if (*currentIndex == livesInd)
        {
            drawMenuHex(TRUE,FALSE,TRUE);
        }
        else if (*currentIndex == sndInd)
        {
            drawMenuHex(TRUE,TRUE,TRUE);
        }
    }
    if (changed & state & BUTTON_START)
    {
        selectOption_Prefs();
    }
}

void preferences()
{
    u16 basetile = TILE_ATTR(PAL3,FALSE,FALSE,FALSE);
    u8 diffStrLen = strlen(menuPrefs[difficulty].label);
    u8 chrStrLen = strlen(menuPrefs[player + 4].label);
    CLEAR_BG1;
    VDP_drawText("Changes made will only take effect",2,0);
    VDP_drawText("upon starting a new game.",2,1);
    VDP_drawText("Difficulty:",optX,optY);
    VDP_drawText("Character:",optX,optY+5);
    VDP_drawText("Lives:",optX,optY+8);
    VDP_drawText("Sound Test:",optX,optY+9);
    drawMenu(menuPrefs,optNum,PAL3,BG_A);
    confCurDiff = SPR_addSprite(&heart,TILE_TO_PIXEL((menuPrefs[difficulty].x + diffStrLen)),TILE_TO_PIXEL(menuPrefs[difficulty].y),basetile);
    confCurChr = SPR_addSprite(&heart,TILE_TO_PIXEL((menuPrefs[player].x + chrStrLen)),TILE_TO_PIXEL(menuPrefs[player + 4].y),basetile);
    musIndex = MEM_alloc(SIZEOF_8BIT);
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