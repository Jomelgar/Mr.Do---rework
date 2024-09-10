#include <cstdio>
#include <iostream>
#include "FontData.h"
#include "TextConsole.h"

void CFontSymbol::loadFromBitmap(const uint8_t *arr) {
    int point_y = 0;

    for (int i = 0; i < SYMB_HEIGHT; i++) {
        uint16_t row_data = getRowData(arr);
        uint16_t mask = 1 << ROW_MSB;

        int point_x = 0;
        for (int j = 0; j < SYMB_WIDTH; j++) {
            if ((row_data & mask) != 0) {
                points.emplace_back(point_x, point_y);
            }            
            point_x ++;
            mask >>= 1;
        }
        arr += BYTES_PER_ROW;
        point_y++;
    }    
}

void CFontSymbol::draw(SDL_Renderer *renderer, int x, int y,
                      const CRGBColor &fc, const CRGBColor &bc) const {
    SDL_Rect r;

    r.x = x; r.y = y; r.w = SYMB_WIDTH; r.h = SYMB_HEIGHT;
    SDL_SetRenderDrawColor(renderer, bc.red, bc.green, bc.blue, 255);
    SDL_RenderFillRect(renderer, &r);
    if (points.size() > 0) {
        std::vector<SDL_Point> spoints(points.size());

        for (int i = 0; i < points.size(); i++) {
            spoints[i].x = x + points[i].x;
            spoints[i].y = y + points[i].y;
        }
        SDL_SetRenderDrawColor(renderer, fc.red, fc.green, fc.blue, 255);
        SDL_RenderDrawPoints(renderer, spoints.data(), points.size());
    }
}

void CFont::loadFromData(const uint8_t romData[], size_t symbSizeBytes, int count) {
    symbols.resize(count);

    for (int i = 0; i < count; i++) {
        const uint8_t *symbBmp = &romData[i * symbSizeBytes];
        symbols[i].loadFromBitmap(symbBmp);
    }
}

TextConsole::~TextConsole() {
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
    if (guiThread != nullptr) {
        guiThread->join();
        delete guiThread;
    }
}

unsigned TextConsole::getWidth() 
{ 
    return cols * SYMB_WIDTH; 
}

unsigned TextConsole::getHeight() 
{ 
    return rows * SYMB_HEIGHT; 
}

bool TextConsole::activate()
{
    font.loadFromData(font_data, SYMB_SIZE_BYTES, SYMB_COUNT);
    frameBuffer.resize(rows * cols);

    if (!init()) {
        return false;
    }
    active = true;

    return true;
}

bool TextConsole::init() 
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << '\n';
        return 1;
    }
    int res = SDL_CreateWindowAndRenderer(getWidth(), getHeight(), SDL_WINDOW_SHOWN, &window, &renderer);
    if (res < 0) {
        std::cerr << "Could not create window: '" << SDL_GetError() << "\n";
        return false;        
    }
    SDL_SetWindowTitle(window, "Console");

    return true;
}

void TextConsole::scrollScreen()
{
    std::copy(frameBuffer.begin() + cols, frameBuffer.end(), frameBuffer.begin());
    unsigned offset = (rows - 1) * cols;

    for (int i = offset; i < offset + cols; i++) {
        frameBuffer[i].foreColor = foreColor;
        frameBuffer[i].backColor = backColor;
        frameBuffer[i].symbol = ' ';
    }
}

void TextConsole::clearScreen()
{
    for (auto& fbcell : frameBuffer) {
        fbcell.foreColor = foreColor;
        fbcell.backColor = backColor;
        fbcell.symbol = ' ';
    }
    setCursor(0, 0);
}

void TextConsole::writeChar(char ch) 
{
    if (ch == '\n') {
        moveCursorToNextLine();
    } else {
        unsigned offset = currentRow * cols + currentCol;
        auto& fbcell = frameBuffer[offset];
        fbcell.foreColor = foreColor;
        fbcell.backColor = backColor;
        fbcell.symbol = ch;
        
        if (currentCol < (getColCount() - 1)) {
            currentCol++;
        } else {
            moveCursorToNextLine();
        }
    }
}

void TextConsole::writeString(const char *str)
{
    const char *p = str;
    while (*p != '\0') {
        writeChar(*p);
        p++;
    }
    postRepaintEvent();
}

void TextConsole::delayMs(unsigned ms)
{
    uint32_t endtime = getMillis() + ms;

    while (getMillis() < endtime) {}
}

uint32_t TextConsole::getKey()
{
    if (keyBuffer.hasKey()) {
        return keyBuffer.popKey();
    } else {
        return SDLK_UNKNOWN;
    }
}

void TextConsole::drawSymbol(int row, int col, const FBCell& fbcell) {
    int index = fbcell.symbol;
    CRGBColor fc = fbcell.foreColor;
    CRGBColor bc = fbcell.backColor;

    const CFontSymbol& s = font[index];

    s.draw(renderer, col * SYMB_WIDTH, row * SYMB_HEIGHT, fc, bc);
}

void TextConsole::drawContent() {
    if (font.size() == 0 || frameBuffer.empty()) {
        return;
    }

    FBCell *pcell = frameBuffer.data();
    
    for (int row = 0; row < rows; row++) {
        int col = 0;

        while (col < cols) {
            drawSymbol(row, col, *pcell);
            col++;
            pcell++;
        }
    }
}

void TextConsole::postRepaintEvent() {
    SDL_Event e;
    e.type = SDL_WINDOWEVENT;
    e.window.event = SDL_WINDOWEVENT_EXPOSED;
    e.window.windowID = SDL_GetWindowID(window);
    SDL_PushEvent(&e);
}

void TextConsole::redraw() {
    drawContent();

    SDL_RenderPresent(renderer);
}

void TextConsole::saveScreenshot(const char *filename) {
    SDL_Surface *pScreenShot;

    pScreenShot = SDL_CreateRGBSurface(0, getWidth(), getHeight(), 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

    if(pScreenShot != nullptr) {
        SDL_RenderReadPixels(renderer, nullptr, SDL_GetWindowPixelFormat(window), pScreenShot->pixels, pScreenShot->pitch);
        SDL_SaveBMP(pScreenShot, filename);
        SDL_FreeSurface(pScreenShot);
    }
}

void TextConsole::run(CClient& cl) 
{
    bool running = true;
    SDL_Event e;

    std::thread cthread(&CClient::run, &cl);

    while (running) {
        SDL_WaitEvent(&e);

        switch (e.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_EXPOSED) {
                    redraw();
                }
                break;
            case SDL_KEYDOWN:
                keyBuffer.addKey(e.key.keysym.sym);
                break;
            case SDL_KEYUP:
                break;
        }
    }
    active = false;
    cthread.join();
}
