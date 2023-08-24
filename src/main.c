#include "includes.h"

u8 palFadeTime;
u8 vblankRate;
bool isNTSC;
const u8* region = (u8 *)0x00A10001;
const u8 titleX = 7;
const u8 titleY = 23;
const u8 numOptsTitle = 4;
const Option menuTitle[] = 
{
    {titleX,titleY,"Start Game"},
    {titleX,titleY+1,"Continue Game"},
    {titleX+15,titleY,"Statistics"},
    {titleX+15,titleY+1,"Preferences"}
};
Sprite* menuCursor;
u8* currentIndex;
u8* musIndex;
fix16* scroll;
fix16* scrollSpeed;

void fadePalette(const u16* palette)
{
    u16 finalPalette[64];
    memcpy(finalPalette,palette,48*SIZEOF_16BIT);
    memcpy(&finalPalette[48],playerPalettes[1],16*SIZEOF_16BIT);
    PAL_fadeInAll(finalPalette,palFadeTime,TRUE);
}

void drawMenu(const Option* option, u8 length, u8 paletteLine, u8 plane)
{
    VDP_setTextPalette(paletteLine);
    VDP_setTextPlane(plane);
    for (u8 i = 0; i < length; i++)
    {
        Option o = option[i];
        VDP_drawText(o.label,o.x,o.y);
    }
    u16 basetile = TILE_ATTR(paletteLine,FALSE,FALSE,FALSE);
    currentIndex = MEM_alloc(SIZEOF_8BIT);
    menuCursor = SPR_addSprite(&cursor,TILE_TO_PIXEL(option[*currentIndex].x) - 8,TILE_TO_PIXEL(option[*currentIndex].y),basetile);
}

void curMove(const Option* option, u8 length, bool direction)
{
    SELECTION_SFX;
    if (!direction)
    {
        if (*currentIndex == 0)
        {
            *currentIndex = length - 1;
        }
        else
        {
            *currentIndex -= 1;
        }
    }
    else
    {
        if (*currentIndex == length - 1)
        {
            *currentIndex = 0;
        }
        else
        {
            *currentIndex += 1;
        }
    }
    SPR_setPosition(menuCursor,TILE_TO_PIXEL(option[*currentIndex].x) - 8,TILE_TO_PIXEL(option[*currentIndex].y));
}

static void selectOption_Title()
{
    SELECTION_SFX;
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
    switch (*currentIndex)
    {
    case 3:
    {
        MEM_free(currentIndex);
        currentIndex = NULL;
        preferences();
        break;
    }
    default:
    {
        killExec(menuIndexInvalid);
        break;
    }
    }
}

static void joyEvent_TitleMenu(u16 joy, u16 changed, u16 state)
{
    if (joy != JOY_1)
    {
        return;
    }
    if (changed & state & BUTTON_UP)
    {
        curMove(menuTitle,numOptsTitle,FALSE);
    }
    else if (changed & state & BUTTON_DOWN)
    {
        curMove(menuTitle,numOptsTitle,TRUE);
    }
    if (changed & state & BUTTON_START)
    {
        selectOption_Title();
    }
}

static void joyEvent_Title(u16 joy, u16 changed, u16 state)
{
    if (joy != JOY_1)
    {
        return;
    }
    if (changed & state & BUTTON_START)
    {
        VDP_clearTextLine(14);
        MDS_request(MDS_SE1,BGM_SFX_S1SELECT);
        drawMenu(menuTitle,4,PAL3,BG_A);
        JOY_setEventHandler(joyEvent_TitleMenu);
    }
}

void title()
{
    s16 indLogo = TILE_USER_INDEX;
    s16 indBG = indLogo + title_logo.tileset->numTile;
    u16 basetileLogo = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,indLogo);
    u16 basetileBG = TILE_ATTR_FULL(PAL1,FALSE,FALSE,FALSE,indBG);
    u8 y;
    scroll = MEM_alloc(SIZEOF_16BIT);
    scrollSpeed = MEM_alloc(SIZEOF_16BIT);
    if (isNTSC)
    {
        y = 27;
        *scrollSpeed = FIX16(1.0/3.0);
    }
    else
    {
        y = 29;
        *scrollSpeed = FIX16(0.4);
    }
    CLEAR_BG1;
    CLEAR_BG2;
    VDP_drawImageEx(BG_A,&title_logo,basetileLogo,0,0,FALSE,TRUE);
    VDP_drawImageEx(BG_B,&title_bg,basetileBG,0,0,FALSE,TRUE);
    VDP_setTextPalette(PAL3);
    VDP_drawText("}TheWindowsPro98 2023",9,y);
    VDP_drawText("Version ppa1.3",0,6);
    VDP_drawText("Press START button",11,14);
    fadePalette(titlePalette);
    JOY_setEventHandler(joyEvent_Title);
    MDS_request(MDS_BGM,BGM_MUS_CLI2);
    while (1)
    {
        *scroll -= *scrollSpeed;
        SYS_doVBlankProcess();
        SPR_update();
        MDS_update();
        VDP_setHorizontalScroll(BG_B,fix16ToInt(*scroll));
    }
}

static void segaScreen()
{
    s16 indFG = TILE_USER_INDEX;
    s16 indBG = indFG + sega_fg.tileset->numTile;
    u16 basetileFG = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,indFG);
    u16 basetileBG = TILE_ATTR_FULL(PAL1,FALSE,FALSE,FALSE,indBG);
    u8 timer = (1.65 * vblankRate) + (palFadeTime << 1);
    u8 y;
    if (isNTSC)
    {
        y = 12;
    }
    else
    {
        y = 13;
    }
    VDP_drawImageEx(BG_A,&sega_fg,basetileFG,0,!isNTSC,FALSE,TRUE);
    VDP_drawImageEx(BG_B,&sega_bg,basetileBG,13,y,FALSE,TRUE);
    fadePalette(segaPalette);
    while (1)
    {
        timer--;
        SYS_doVBlankProcess();
        MDS_update();
        if (timer == 129)
        {
            MDS_request(MDS_BGM,BGM_SFX_SEGA);
        }
        else if (timer == 30)
        {
            PAL_FADE_OUT;
        }
        else if (timer == 0)
        {
            PAL_interruptFade();
            title();
        }
    }
}

int main(bool resetType)
{
    if (*region == ntscJPN || *region == ntscUSA)
    {
        palFadeTime = 30;
        vblankRate = 60;
        isNTSC = TRUE;
    }
    else if (*region == palEUR)
    {
        VDP_setScreenHeight240();
        palFadeTime = 25;
        vblankRate = 50;
        isNTSC = FALSE;
    }
    else
    {
        killExec(regionInvalid);
    }
    for (u8 i = 0; i < 4; i++)
    {
        PAL_setPalette(i,palette_black,DMA);
    }
    Z80_unloadDriver();
    VDP_loadFont(&main_font,DMA);
    MDS_init(mdsseqdat,mdspcmdat);
    SPR_init();
    segaScreen();
    while(1)
    {
        SYS_doVBlankProcess();
        MDS_update();
    }
    return (0);
}
