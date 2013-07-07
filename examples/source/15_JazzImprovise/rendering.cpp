
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

SGDI* CRendering::sGDI;

char CRendering::sHeader[32];

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

    sGDI->hFontBig = CreateFont(22, 8, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, "Times New Roman");

    // create brushes
    for(int i = 0; i < 128; i++)
        sGDI->hKeyBrush[i] = CreateSolidBrush(RGB(255, 255 - (i * 2), 255 - (i * 2)));

    sGDI->hButtonsBrush = CreateSolidBrush(RGB(246, 230, 199));

    // create bitmaps
    sGDI->bmpWhiteKey = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\pianokey_white.bmp", IMAGE_BITMAP, 0, 0, 
                                           LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcWhiteKey = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcWhiteKey, sGDI->bmpWhiteKey);

    sGDI->bmpBlackKey = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\pianokey_black.bmp", IMAGE_BITMAP, 0, 0, 
                                           LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcBlackKey = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcBlackKey, sGDI->bmpBlackKey);

    sGDI->bmpWhiteKeyDisabled = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\pianokey_white_disabled.bmp", IMAGE_BITMAP, 0, 0, 
                                                   LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcWhiteKeyDisabled = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcWhiteKeyDisabled, sGDI->bmpWhiteKeyDisabled);

    sGDI->bmpBlackKeyDisabled = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\pianokey_black_disabled.bmp", IMAGE_BITMAP, 0, 0, 
                                                   LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcBlackKeyDisabled = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcBlackKeyDisabled, sGDI->bmpBlackKeyDisabled);

    sGDI->bmpPedalOn = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\pianopedal_on.bmp", IMAGE_BITMAP, 0, 0, 
                                          LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcPedalOn = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcPedalOn, sGDI->bmpPedalOn);

    sGDI->bmpPedalOff = (HBITMAP)LoadImage(NULL, ".\\..\\..\\common\\images\\pianopedal_off.bmp", IMAGE_BITMAP, 0, 0, 
                                           LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    sGDI->hdcPedalOff = CreateCompatibleDC(NULL);
    SelectObject(sGDI->hdcPedalOff, sGDI->bmpPedalOff);
}

void CRendering::render(SGlobal* Global)
{
    int i;

    int iKeyX;
    int iKeyY = PIANO_Y;

    // background
    SelectObject(sGDI->hdcmem, (HBRUSH)GetStockObject(WHITE_BRUSH));
    Rectangle(sGDI->hdcmem, 0, 0, sGDI->ClientX, sGDI->ClientY);

    // header
    drawText(16, 12, sHeader, sGDI->hFontText, COLORREF(0x00));
    drawText(16, 30, "C++ Music Library", sGDI->hFontText, COLORREF(0x00));
    drawText(16, 48, "Sample application", sGDI->hFontText, COLORREF(0x00));

    if(!Global->mMusicTimer->isRunning())
        drawText(16, 86, "Press S to start!", sGDI->hFontBig, RGB(0, 0, 0));

    // tempo
    sprintf(sGDI->sBuffer, "Tempo: %.0f    ", Global->mMusicTimer->getTempo());
    drawText(320, 35, sGDI->sBuffer, sGDI->hFontBig, RGB(156, 156, 156));

    // measure
    sprintf(sGDI->sBuffer, "Measure: %d    ", Global->mMusicTimer->getTimePosition().Measure);
    drawText(320, 60, sGDI->sBuffer, sGDI->hFontBig, RGB(156, 156, 156));

    // beat
    sprintf(sGDI->sBuffer, "Beat: %d    ", Global->mMusicTimer->getTimePosition().Beat);
    drawText(320, 85, sGDI->sBuffer, sGDI->hFontBig, RGB(156, 156, 156));

    // current root note
    MCNotes::whichNote(Global->mHarmonyPattern->getEntry(Global->iHarmonyPatternPosition)->RootNote, sGDI->sNote);
    sprintf(sGDI->sBuffer, "Root note: %s    ", sGDI->sNote);
    drawText(470, 35, sGDI->sBuffer, sGDI->hFontBig, RGB(156, 156, 156));

    // current scale pattern
    sprintf(sGDI->sBuffer, "Scale pattern: %d    ", Global->iCurrentImpScalePattern + 1);
    drawText(470, 60, sGDI->sBuffer, sGDI->hFontBig, RGB(156, 156, 156));

    // current part
    if(Global->mMusicTimer->getTimePosition().Measure <= LOOP)
    {
        strcpy(sGDI->sBuffer, "Current part: theme");
    }
    else
    {
        if(Global->bEndingTheme)
        {
            if(Global->mMusicTimer->getTimePosition().Measure <= Global->iMeasureEndingTheme)
                sprintf(sGDI->sBuffer, "Current part: going to the theme (measure %d)", Global->iMeasureEndingTheme);
            else
                strcpy(sGDI->sBuffer, "Current part: ending theme");
        }
        else
        {
            sprintf(sGDI->sBuffer, "Current part: piano solo #%d", (Global->mMusicTimer->getTimePosition().Measure - 1) / LOOP);
        }
    }

    drawText(470, 85, sGDI->sBuffer, sGDI->hFontBig, RGB(156, 156, 156));

    // keyboard map
    drawText(60, PIANO_Y + WHITEKEY_HEIGHT + 65, "Playing:", sGDI->hFontBold, COLORREF(0x00));
    drawText(60, PIANO_Y + WHITEKEY_HEIGHT + 90, "- Q,W,E,R,T,Y,U,I,O,P: play solo note", sGDI->hFontText, COLORREF(0x00));
    drawText(60, PIANO_Y + WHITEKEY_HEIGHT + 110, "- SPACE: play voicing (harmony)", sGDI->hFontText, COLORREF(0x00));
    drawText(60, PIANO_Y + WHITEKEY_HEIGHT + 130, "- LEFT/RIGHT: change keyboard solo scope", sGDI->hFontText, COLORREF(0x00));
    drawText(60, PIANO_Y + WHITEKEY_HEIGHT + 150, "- 1,2,3: select set of scales", sGDI->hFontText, COLORREF(0x00));

    drawText(380, PIANO_Y + WHITEKEY_HEIGHT + 65, "Communication with band:", sGDI->hFontBold, COLORREF(0x00));
    drawText(380, PIANO_Y + WHITEKEY_HEIGHT + 90, "- UP: \"faster!\"", sGDI->hFontText, COLORREF(0x00));
    drawText(380, PIANO_Y + WHITEKEY_HEIGHT + 110, "- DOWN: \"slower!\"", sGDI->hFontText, COLORREF(0x00));
    drawText(380, PIANO_Y + WHITEKEY_HEIGHT + 130, "- M: \"let's go to the theme and finish\"", sGDI->hFontText, COLORREF(0x00));

    drawText(680, PIANO_Y + WHITEKEY_HEIGHT + 65, "Application:", sGDI->hFontBold, COLORREF(0x00));
    drawText(680, PIANO_Y + WHITEKEY_HEIGHT + 90, "- D: change MIDI in device", sGDI->hFontText, COLORREF(0x00));
    drawText(680, PIANO_Y + WHITEKEY_HEIGHT + 110, "- ESC: quit", sGDI->hFontText, COLORREF(0x00));

    // MIDI device
    if(Global->iMIDINumDevices > 0)
    {
        sprintf(sGDI->sBuffer, "MIDI device: %s", Global->sMIDIDevices[Global->iMIDISelectedDevice]);
        drawText(16, PIANO_Y + WHITEKEY_HEIGHT + 6, sGDI->sBuffer, sGDI->hFontBold, COLORREF(0x0000BB));
    }

    // keyboard buttons scope
    MTNote mCurrentNote;
    MTNote mFirstNote;
    MTNote mLastNote;

    mFirstNote = max(Global->mNoteMap[Global->iCursor], Global->mPianoRange.LowestNote);
    mLastNote = min(Global->mNoteMap[Global->iCursor + PAD_BUTTONS - 1], Global->mPianoRange.HighestNote);

    // pressed keys and keyboard scope
    int iNote;
    int iOctave;
    int iOffset;

    int iFirstNoteX;
    int iLastNoteX;

    // first loop: white keys
    for(i = 0; i < 88; i++)
    {
        if(!MCNotes::isNatural(i + LOWEST_NOTE))
            continue;

        mCurrentNote = i + Global->mPianoRange.LowestNote;

        // locate key position
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

        // white key image
        if(MCNoteMaps::contains(Global->mNoteMap, mCurrentNote))
        {
            drawBMP(iKeyX - (WHITEKEY_WIDTH / 2), PIANO_Y, WHITEKEY_WIDTH, WHITEKEY_HEIGHT, 
                    sGDI->hdcWhiteKey, sGDI->bmpWhiteKey);
        }
        else
        {
            drawBMP(iKeyX - (WHITEKEY_WIDTH / 2), PIANO_Y, WHITEKEY_WIDTH, WHITEKEY_HEIGHT, 
                    sGDI->hdcWhiteKeyDisabled, sGDI->bmpWhiteKeyDisabled);
        }

        // key pressed
        if(Global->bKeyPressed[i])
        {
            SelectObject(sGDI->hdcmem, (HBRUSH)sGDI->hKeyBrush[Global->iKeyIntensity[i]]);
            Ellipse(sGDI->hdcmem, iKeyX - 5, iKeyY - 5, iKeyX + 5, iKeyY + 5);
        }

        // buttons to play solo
        if(mCurrentNote == mFirstNote)
            iFirstNoteX = iKeyX - (WHITEKEY_WIDTH / 2);        
        else if(mCurrentNote == mLastNote)
            iLastNoteX = iKeyX + (WHITEKEY_WIDTH / 2) + 1;
    }

    // second loop: black keys
    for(i = 0; i < 88; i++)
    {
        if(MCNotes::isNatural(i + LOWEST_NOTE))
            continue;

        mCurrentNote = i + Global->mPianoRange.LowestNote;

        // locate key position
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

        // black key image
        if(MCNoteMaps::contains(Global->mNoteMap, mCurrentNote))
        {
            drawBMP(iKeyX - (WHITEKEY_WIDTH / 2), PIANO_Y, BLACKKEY_WIDTH, BLACKKEY_HEIGHT, 
                    sGDI->hdcBlackKey, sGDI->bmpBlackKey);
        }
        else
        {
            drawBMP(iKeyX - (WHITEKEY_WIDTH / 2), PIANO_Y, BLACKKEY_WIDTH, BLACKKEY_HEIGHT, 
                    sGDI->hdcBlackKeyDisabled, sGDI->bmpBlackKeyDisabled);
        }

        // key pressed
        if(Global->bKeyPressed[i])
        {
            SelectObject(sGDI->hdcmem, (HBRUSH)sGDI->hKeyBrush[Global->iKeyIntensity[i]]);
            Ellipse(sGDI->hdcmem, iKeyX - 5, iKeyY - 5, iKeyX + 5, iKeyY + 5);
        }

        // buttons to play solo
        if(mCurrentNote == mFirstNote)
            iFirstNoteX = iKeyX - (WHITEKEY_WIDTH / 2);        
        else if(mCurrentNote == mLastNote)
            iLastNoteX = iKeyX + (WHITEKEY_WIDTH / 2) - 2;
    }
    
    // keyboard zone
    SelectObject(sGDI->hdcmem, (HBRUSH)sGDI->hButtonsBrush);
    Rectangle(sGDI->hdcmem, iFirstNoteX, PIANO_Y - 25, iLastNoteX, PIANO_Y + 1);
    drawText((iFirstNoteX + iLastNoteX) / 2 - 40, PIANO_Y - 21, "Keyboard Q-P", sGDI->hFontText, COLORREF(0x00));

    // damper pedal
    drawBMP((sGDI->WindowSizeX / 2) - (PEDAL_WIDTH / 2), PIANO_Y + WHITEKEY_HEIGHT, PEDAL_WIDTH, PEDAL_HEIGHT, 
            (Global->bDamper)? sGDI->hdcPedalOn: sGDI->hdcPedalOff, 
            (Global->bDamper)? sGDI->bmpPedalOn: sGDI->bmpPedalOff);
}

void CRendering::release()
{
    DeleteObject(sGDI->hFontText);
    DeleteObject(sGDI->hFontBold);
    DeleteObject(sGDI->hFontBig);

    DeleteObject(sGDI->bmp);
    DeleteObject(sGDI->bmpWhiteKey);
    DeleteObject(sGDI->bmpWhiteKeyDisabled);
    DeleteObject(sGDI->bmpBlackKey);
    DeleteObject(sGDI->bmpBlackKeyDisabled);
    DeleteObject(sGDI->bmpPedalOn);
    DeleteObject(sGDI->bmpPedalOff);

    DeleteDC(sGDI->hdcWhiteKey);
    DeleteDC(sGDI->hdcWhiteKeyDisabled);
    DeleteDC(sGDI->hdcBlackKey);
    DeleteDC(sGDI->hdcBlackKeyDisabled);
    DeleteDC(sGDI->hdcPedalOn);
    DeleteDC(sGDI->hdcPedalOff);
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
