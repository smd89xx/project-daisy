#include "includes.h"

const string stopStrings[] = {"ERROR_USER_GENERIC","ERROR_FUNCTION_UNIMPLEMENTED","ERROR_REGION_INVALID","ERROR_MENU_INDEX_INVALID","ERROR_LEVEL_INVALID","ERROR_PLAYER_INVALID"};
u32* stopcode_public;

static void joyEvent_BSOD(u16 joy, u16 changed, u16 state)
{
    if (joy != JOY_1)
    {
        return;
    }
    if (changed & state & BUTTON_START)
    {
        string unfixableErrorStr = "Error is unfixable!";
        VDP_drawText("Fixing errors...",0,22);
        MDS_request(MDS_BGM,BGM_SFX_S1SELECT);
        switch (*stopcode_public)
        {
        case 2:
        {
            VDP_drawText(unfixableErrorStr,0,22);
            break;
        }
        default:
        {
            SYS_hardReset();
            break;
        }
        }
    }
}

void killExec(u32 stopcode)
{
    u8 i = 0;
    for (i; i < 4; i++)
    {
        PAL_setPalette(i,palette_black,DMA);
    }
    VDP_loadFont(&bsodFont,DMA);
    CLEAR_BG1;
    CLEAR_BG2;
    VDP_setWindowVPos(FALSE,0);
    SPR_end();
    PAL_interruptFade();
    VDP_setTextPalette(PAL0);
    string bsodStrings[14] = {"A problem has been detected and the","current process has ended to prevent","damage to your console.","If this is the first time you've seen","this screen, reset your console. If you","see this screen again, press the","START button. This will make an attempt","to fix any problems you are having.","If problems continue, contact the","developer for a potential fix, if any","can be provided, or erase all save data","and start over. A backup is recommended.","Technical Information:","***** STOP: 0x"};
    u32 errPtr;
    u32 errVal;
    for (i = 0; i < 3; i++)
    {
        VDP_drawText(bsodStrings[i],0,i);
    }
    VDP_drawText(stopStrings[stopcode],0,4);
    for (i = 3; i < 8; i++)
    {
        VDP_drawText(bsodStrings[i],0,i+3);
    }
    for (i = 8; i < 12; i++)
    {
        VDP_drawText(bsodStrings[i],0,i+4);
    }
    for (i = 12; i < 14; i++)
    {
        VDP_drawText(bsodStrings[i],0,i+5);
    }
    switch (stopcode)
    {
    case 2:
    {
        errPtr = (u32)region;
        errVal = (u32)*region;
        break;
    }
    case 3:
    {
        errPtr = (u32)currentIndex;
        errVal = (u32)*currentIndex;
        break;
    }
    case 4:
    {
        errPtr = (u32)&level;
        errVal = (u32)level;
        break;
    }
    case 5:
    {
        errPtr = (u32)&player;
        errVal = (u32)player;
        break;
    }
    default:
    {
        errPtr = NULL;
        errVal = NULL;
        break;
    }
    }
    char scStr[9] = "DEADDEAD";
    intToHex(stopcode,scStr,8);
    VDP_drawText(scStr,14,18);
    intToHex(errPtr,scStr,8);
    VDP_drawText(scStr,14,19);
    intToHex(errVal,scStr,8);
    VDP_drawText(scStr,14,20);
    VDP_drawText("** ADDRESS: 0x",0,19);
    VDP_drawText("** CONTENT: 0x",0,20);
    PAL_setPalette(PAL0,bsodPalette,DMA);
    MUSIC_FADE;
    stopcode_public = MEM_alloc(SIZEOF_32BIT);
    memcpy(stopcode_public,&stopcode,SIZEOF_32BIT);
    JOY_setEventHandler(joyEvent_BSOD);
    while (1)
    {
        SYS_doVBlankProcess();
    }
}