#include "includes.h"

u8 level;
Sprite* playerSpr;
Map* lvlFG;
Map* lvlBG;
fix32 player_x;
fix32 player_y;
fix16 player_x_spd;
fix16 player_y_spd;
s32 cam_x;
s32 cam_y;
s32 mapWidths[] = {1280};
s32 mapHeights[] = {720};

enum cameraBoundaries {leftCamBnd = 144, rightCamBnd = 144, topCamBnd = 104, bottomCamBnd = 104};
enum playerDimensions {playerWidth = 32, playerHeight = 32};

static void camPos()
{
    s32 px = fix32ToInt(player_x);
    s32 py = fix32ToInt(player_y);
    s32 px_scrn = px - cam_x;
    s32 py_scrn = py - cam_y;
    if (px_scrn > rightCamBnd)
    {
        cam_x = px - rightCamBnd;
    }
    else if (px_scrn < leftCamBnd)
    {
        cam_x = px - leftCamBnd;
    }
    if (py_scrn > bottomCamBnd)
    {
        cam_y = py - bottomCamBnd;
    }
    else if (py_scrn < topCamBnd)
    {
        cam_y = py - topCamBnd;
    }
    if (cam_x < 0)
    {
        cam_x = 0;
    }
    else if (cam_x > mapWidths[level] - screenWidth)
    {
        cam_x = mapWidths[level] - screenWidth;
    }
    if (cam_y < -16)
    {
        cam_y = -16;
    }
    else if (cam_y > mapHeights[level] - screenHeight)
    {
        cam_y = mapHeights[level] - screenHeight;
    }
    MAP_scrollTo(lvlFG,cam_x,cam_y);
    MAP_scrollTo(lvlBG,cam_x >> 1,cam_y >> 1);
}

static void spawnPlayer()
{
    u16 basetile = TILE_ATTR(PAL3,FALSE,FALSE,FALSE);
    s32 px = fix32ToInt(player_x);
    s32 py = fix32ToInt(player_y);
    u16 ind;
    switch (level)
    {
        case 0:
        {
            ind = TILE_USER_INDEX + test_tiles.numTile;
            break;
        }
        default:
        {
            break;
        }
    }
    switch (player)
    {
    case TRUE:
    {
        playerSpr = SPR_addSprite(&stephanie,px,py,basetile);
        break;
    }
    default:
    {
        killExec(plrInvalid);
        break;
    }
    }
    SPR_setVRAMTileIndex(playerSpr,ind);
}

static void initMap()
{
    MAP_scrollTo(lvlBG,0,0);
    SYS_doVBlankProcess();
    MAP_scrollTo(lvlFG,0,0);
    MEM_free(lvlFG);
    MEM_free(lvlBG);
}

static u8* drawLevel()
{
    s16 ind = TILE_USER_INDEX;
    u16 basetile = TILE_ATTR_FULL(PAL0,FALSE,FALSE,FALSE,ind);
    switch (level)
    {
    case 0:
    {
        VDP_loadTileSet(&test_tiles,ind,DMA);
        lvlFG = MAP_create(&testlvl_fg,BG_A,basetile);
        lvlBG = MAP_create(&testlvl_bg,BG_B,basetile);
        initMap();
        player_x = FIX32(8);
        player_y = FIX32(144);
        MDS_request(MDS_BGM,BGM_MUS_CLI2);
        fadePalette(testlvlPalette);
        return TESTLVL_COLLISION;
        break;
    }
    default:
    {
        killExec(lvlInvalid);
        break;
    }
    }
}

static void playerMove()
{
    player_x += fix16ToFix32(player_x_spd);
    player_y += fix16ToFix32(player_y_spd);
    s32 px = fix32ToInt(player_x);
    s32 py = fix32ToInt(player_y);
    if (px < 0)
    {
        player_x = FIX32(0);
    }
    else if (px > mapWidths[level] - playerWidth)
    {
        player_x = FIX32(mapWidths[level] - playerWidth);
    }
    if (py < 0)
    {
        player_y = FIX32(0);
    }
    else if (py > mapHeights[level] - playerHeight)
    {
        player_y = FIX32(mapHeights[level] - playerHeight);
    }
    SPR_setPosition(playerSpr,px - cam_x,py - cam_y);
}

static void joyEvent_Game(u16 joy, u16 changed, u16 state)
{
    if (joy != JOY_1)
    {
        return;
    }
    if (state & BUTTON_LEFT)
    {
        SPR_setHFlip(playerSpr,TRUE);
        player_x_spd = FIX16(-1.5);
    }
    else if (state & BUTTON_RIGHT)
    {
        SPR_setHFlip(playerSpr,FALSE);
        player_x_spd = FIX16(1.5);
    }
    else
    {
        player_x_spd = FIX16(0);
    }
    if (state & BUTTON_UP)
    {
        player_y_spd = FIX16(-1.5);
    }
    else if (state & BUTTON_DOWN)
    {
        player_y_spd = FIX16(1.5);
    }
    else
    {
        player_y_spd = FIX16(0);
    }
}

static void initHUD()
{
    VDP_setWindowVPos(FALSE,2);
    VDP_setTextPlane(WINDOW);
    VDP_drawText("PX:",0,0);
    VDP_drawText("PY:",0,1);
    VDP_drawText("CX:",12,0);
    VDP_drawText("CY:",12,1);
}

static void drawIntToHex(u32 value, char str[], u8 len, u8 x, u8 y)
{
    intToHex(value,str,len);
    VDP_drawTextBG(WINDOW,str,x,y);
}

static void updateHUD()
{
    char pxStr[9];
    char pyStr[9];
    char cxStr[9];
    char cyStr[9];
    s32 px = fix32ToInt(player_x);
    s32 py = fix32ToInt(player_y);
    drawIntToHex(px,pxStr,8,3,0);
    drawIntToHex(py,pyStr,8,3,1);
    drawIntToHex(cam_x,cxStr,8,15,0);
    drawIntToHex(cam_y,cyStr,8,15,1);
}

void gameInit()
{
    CLEAR_BG1;
    CLEAR_BG2;
    SPR_reset();
    u8* collsionIndex = drawLevel();
    initHUD();
    spawnPlayer();
    JOY_setEventHandler(joyEvent_Game);
    while(1)
    {
        SPR_update();
        MDS_update();
        playerMove();
        camPos();
        updateHUD();
        SYS_doVBlankProcess();
    }
}