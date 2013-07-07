
////////////////////////////////////////////////////////////////////////
//
//  Modus
//  C++ Music Library
//  (Examples)
//
//  Arturo Cepeda Pérez
//  July 2012
//
////////////////////////////////////////////////////////////////////////

#include "rendering.h"
#include "mconstants.h"
#include <windows.h>

SGDI* CRendering::sGDI;

char CRendering::sHeader[32];
char CRendering::sFinger[2];

void CRendering::init(SGDI* GDI)
{
    sGDI = GDI;
    HDC hdc;

    // Modus header
    sprintf(sHeader, "Modus %s", MODUS_VERSION);

    // define the rectangle to render
    GetClientRect(sGDI->hWnd, &(sGDI->rArea));
    sGDI->ClientX = sGDI->rArea.right;
    sGDI->ClientY = sGDI->rArea.bottom;

    // create the appropriate objets for double buffering
    sGDI->hdcmem = CreateCompatibleDC(NULL);
    hdc = GetDC(sGDI->hWnd);
    sGDI->bmp = CreateCompatibleBitmap(hdc, sGDI->ClientX, sGDI->ClientY);
    sGDI->bmpOld = (HBITMAP)SelectObject(sGDI->hdcmem, sGDI->bmp);
    ReleaseDC(sGDI->hWnd, hdc);

    // create fonts
    sGDI->hFontText = CreateFont(18, 6, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, "Times New Roman");

    sGDI->hFontBold = CreateFont(18, 6, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, "Times New Roman");

    sGDI->hFontKeys = CreateFont(26, 12, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, "Times New Roman");

    // create bitmaps
    sGDI->bmpWhiteKey = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\pianokey_white.bmp", IMAGE_BITMAP, 0, 0, 
                                           LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcWhiteKey = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcWhiteKey, sGDI->bmpWhiteKey);

    sGDI->bmpBlackKey = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\pianokey_black.bmp", IMAGE_BITMAP, 0, 0, 
                                           LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcBlackKey = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcBlackKey, sGDI->bmpBlackKey);

    sGDI->bmpPedalOn = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\pianopedal_on.bmp", IMAGE_BITMAP, 0, 0, 
                                          LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcPedalOn = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcPedalOn, sGDI->bmpPedalOn);

    sGDI->bmpPedalOff = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\pianopedal_off.bmp", IMAGE_BITMAP, 0, 0, 
                                           LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcPedalOff = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcPedalOff, sGDI->bmpPedalOff);

    sGDI->bmpHands = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\hands.bmp", IMAGE_BITMAP, 0, 0, 
                                        LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcHands = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcHands, sGDI->bmpHands);

    // finger string
    sFinger[1] = '\0';
}

void CRendering::render(SGlobal* Global)
{
    int i;

    int iKeyX;
    int iKeyY = PIANO_Y;

    int iNote;
    int iOctave;
    int iOffset;

    COLORREF cColor;

    // background
    SelectObject(sGDI->hdcmem, (HBRUSH)GetStockObject(WHITE_BRUSH));
    Rectangle(sGDI->hdcmem, 0, 0, sGDI->ClientX, sGDI->ClientY);

    // header
    drawText(16, 12, sHeader, sGDI->hFontText, COLORREF(0x00));
    drawText(16, 30, "C++ Music Library", sGDI->hFontText, COLORREF(0x00));
    drawText(16, 48, "Sample application", sGDI->hFontText, COLORREF(0x00));
    drawText(16, 86, "Playing! Press ESC to quit...", sGDI->hFontBold, COLORREF(0x00));

    // hands diagram
    drawBMP(sGDI->ClientX / 2 - HANDS_WIDTH / 2, 20, HANDS_WIDTH, HANDS_HEIGHT, sGDI->hdcHands, sGDI->bmpHands);

    // first loop: white keys
    for(i = 0; i < 88; i++)
    {
        if(!MCNotes::isNatural(i + LOWEST_NOTE))
            continue;

        iNote = (i + LOWEST_NOTE) % 12;
        iOctave = (i + LOWEST_NOTE) / 12 - 2;   // pitch: [21, 108] --> octave: [0, 7]
        iOffset = 2 + (iOctave * 7);            // initial offset 2: the first C is the 3rd key

        iKeyY = PIANO_Y + (WHITEKEY_HEIGHT * 4 / 5);

        switch(iNote)
        {
        case D:
            iOffset += 1;
            break;
        case E:
            iOffset += 2;
            break;
        case F:
            iOffset += 3;
            break;
        case G:
            iOffset += 4;
            break;
        case A:
            iOffset += 5;
            break;
        case B:
            iOffset += 6;
            break;
        }

        iKeyX = 16 + (WHITEKEY_WIDTH / 2) + (WHITEKEY_WIDTH * iOffset);

        // draw key
        drawBMP(iKeyX - (WHITEKEY_WIDTH / 2), PIANO_Y, WHITEKEY_WIDTH, WHITEKEY_HEIGHT, sGDI->hdcWhiteKey, sGDI->bmpWhiteKey);

        // key pressed
        if(Global->bKeyPressed[i])
        {
            // left hand: blue
            if(Global->iKeyHand[i] == 1)
                cColor = RGB(255 - (Global->iKeyIntensity[i] * 2), 255 - (Global->iKeyIntensity[i] * 2), 255);

            // right hand: red
            else
                cColor = RGB(255, 255 - (Global->iKeyIntensity[i] * 2), 255 - (Global->iKeyIntensity[i] * 2));

            // finger number
            sFinger[0] = Global->iKeyFinger[i] + 48;
            drawText(iKeyX - 7, iKeyY - 12, sFinger, sGDI->hFontKeys, cColor);
        }
    }

    // second loop: black keys
    for(i = 0; i < 88; i++)
    {
        if(MCNotes::isNatural(i + LOWEST_NOTE))
            continue;

        iNote = (i + LOWEST_NOTE) % 12;
        iOctave = (i + LOWEST_NOTE) / 12 - 2;   // pitch: [21, 108] --> octave: [0, 7]
        iOffset = 2 + (iOctave * 7);            // initial offset 2: the first C is the 3rd key

        iKeyY = PIANO_Y + (BLACKKEY_HEIGHT * 2 / 3);
                
        switch(iNote)
        {
        case Cs:
            iOffset += 1;
            break;
        case Ds:
            iOffset += 2;
            break;
        case Fs:
            iOffset += 4;
            break;
        case Gs:
            iOffset += 5;
            break;
        case As:
            iOffset += 6;
            break;
        }

        iKeyX = 16 + (WHITEKEY_WIDTH * iOffset);

        // draw key
        drawBMP(iKeyX - (WHITEKEY_WIDTH / 2), PIANO_Y, BLACKKEY_WIDTH, BLACKKEY_HEIGHT, sGDI->hdcBlackKey, sGDI->bmpBlackKey);

        // key pressed
        if(Global->bKeyPressed[i])
        {
            // left hand: blue
            if(Global->iKeyHand[i] == 1)
                cColor = RGB(255 - (Global->iKeyIntensity[i] * 2), 255 - (Global->iKeyIntensity[i] * 2), 255);

            // right hand: red
            else
                cColor = RGB(255, 255 - (Global->iKeyIntensity[i] * 2), 255 - (Global->iKeyIntensity[i] * 2));

            // finger number
            sFinger[0] = Global->iKeyFinger[i] + 48;
            drawText(iKeyX - 7, iKeyY - 12, sFinger, sGDI->hFontKeys, cColor);
        }
    }

    // damper pedal
    drawBMP((sGDI->WindowSizeX / 2) - (PEDAL_WIDTH / 2), PIANO_Y + WHITEKEY_HEIGHT, PEDAL_WIDTH, PEDAL_HEIGHT, 
            (Global->bDamper)? sGDI->hdcPedalOn: sGDI->hdcPedalOff, 
            (Global->bDamper)? sGDI->bmpPedalOn: sGDI->bmpPedalOff);
}

void CRendering::release()
{
    DeleteObject(sGDI->hFontText);
    DeleteObject(sGDI->hFontBold);
    DeleteObject(sGDI->hFontKeys);

    DeleteObject(sGDI->bmp);
    DeleteObject(sGDI->bmpWhiteKey);
    DeleteObject(sGDI->bmpBlackKey);
    DeleteObject(sGDI->bmpPedalOn);
    DeleteObject(sGDI->bmpPedalOff);
    DeleteObject(sGDI->bmpHands);

    DeleteDC(sGDI->hdcWhiteKey);
    DeleteDC(sGDI->hdcBlackKey);
    DeleteDC(sGDI->hdcPedalOn);
    DeleteDC(sGDI->hdcPedalOff);
    DeleteDC(sGDI->hdcHands);
    DeleteDC(sGDI->hdcmem);
}

void CRendering::drawText(int X, int Y, const char* sText, HFONT hFont, COLORREF cColor)
{
    SetBkMode(sGDI->hdcmem, TRANSPARENT);
    SelectObject(sGDI->hdcmem, hFont);
    SetTextColor(sGDI->hdcmem, cColor);
    TextOut(sGDI->hdcmem, X, Y, sText, strlen(sText));
}

void CRendering::drawBMP(int X, int Y, int iWidth, int iHeight, HDC hdc, HBITMAP bmp)
{
    BitBlt(sGDI->hdcmem, X, Y, iWidth, iHeight, hdc, 0, 0, SRCCOPY);
}
