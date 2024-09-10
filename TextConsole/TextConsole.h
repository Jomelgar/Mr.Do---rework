#ifndef __TEXT_CONSOLE_H__

#define __TEXT_CONSOLE_H__

#include <cstdint>
#include <vector>
#include <deque>
#include <thread>
#include <sstream>
#include <SDL.h>
#include "CColor.h"

struct CPoint {
    CPoint(unsigned x, unsigned y): x(x), y(y) {}

    unsigned x;
    unsigned y;
};

class CFontSymbol {
public:    
    CFontSymbol() {}
    CFontSymbol(const uint8_t *arr) { loadFromBitmap(arr); }

    void loadFromBitmap(const uint8_t *arr);
    void draw(SDL_Renderer *renderer, int x, int y,
              const CRGBColor &fc, const CRGBColor &bc) const;
    size_t getNumOfPoints() const { return points.size(); }

private:
    std::vector<CPoint> points;
};

// Bitmap Font
class CFont {
public:
    CFont() {}

    void loadFromData(const uint8_t data[], size_t symbSizeBytes, int count);

    const CFontSymbol& operator[](size_t index) {
        return symbols[index];
    }

    size_t size() { return symbols.size(); }

private:
    std::vector<CFontSymbol> symbols;
};

struct FBCell {
    CRGBColor foreColor;
    CRGBColor backColor;
    uint8_t symbol;
};

class CKeyBuffer {
public:
    CKeyBuffer() {}

    void addKey(uint32_t key) {
        buffer.push_back(key);
        if (buffer.size() > 8) {
            buffer.pop_front();
        }
    }

    void flush() { 
        buffer.clear();
    }
    
    uint32_t popKey() {
        uint32_t k = buffer.front();
        buffer.pop_front();

        return k;
    }

    bool hasKey() { return !buffer.empty(); }

private:
    std::deque<uint32_t> buffer;
};

class TextConsole;

class CClient {
public:
    CClient(TextConsole& con): con(con) {}

    TextConsole& getConsole() { return con; }
    virtual void run() = 0;

protected:
    TextConsole& con;
};

class TextConsole {
public:
    TextConsole(unsigned cols, unsigned rows):
        window(nullptr),
        renderer(nullptr),
        frameBuffer(),
        guiThread(nullptr),
        active(false),
        currentRow(0),
        currentCol(0),
        rows(rows),
        cols(cols) {}

    ~TextConsole();

    bool activate();
    void saveScreenshot(const char *filename);
    void clearScreen();
    
    void setCursor(unsigned row, unsigned col) {
        currentRow = row;
        currentCol = col;
    }

    void setForecolor(const CRGBColor& color) {
        foreColor = color;
    }

    void setForecolor(CColor color) {
        colorToRGB(color, foreColor);
    }

    void setBackcolor(const CRGBColor& color) {
        backColor = color;
    }

    void setBackcolor(CColor color) {
        colorToRGB(color, backColor);
    }

    void refresh() {
		postRepaintEvent();
	}

    void flushKeybuffer() {
        keyBuffer.flush();
    }

    void writeString(const char *str);
    void delayMs(unsigned ms);
    uint32_t getKey();

    void run(CClient& cl);
    unsigned getWidth();
    unsigned getHeight();
    unsigned getRowCount() { return rows; }
    unsigned getColCount() { return cols; }
    unsigned getCurrentRow() { return currentRow; }
    unsigned getCurrentCol() { return currentCol; }
    CRGBColor getForecolor() { return foreColor; }
    CRGBColor getBackcolor() { return backColor; }
    uint32_t getMillis() { return SDL_GetTicks(); }
    bool isActive() { return active; }

    template <typename T>
    friend TextConsole& operator << (TextConsole& con, const T &);

private:
    bool init();
    void redraw();
    void scrollScreen();
    void writeChar(char ch);
    void postRepaintEvent();
    void drawContent();
    void drawSymbol(int row, int col, const FBCell& fbcell);    

    void moveCursorToNextLine() {
        currentCol = 0;
        if (currentRow < (getRowCount() - 1)) {
            currentRow++;
        } else {
            scrollScreen();
        }
    }

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    std::thread *guiThread;
    CFont font; // Bitmap font
    std::vector<FBCell> frameBuffer;
    CKeyBuffer keyBuffer;
    bool active;
    CRGBColor foreColor;
    CRGBColor backColor;
    unsigned currentRow;
    unsigned currentCol;
    unsigned rows;
    unsigned cols;
};

template <typename T>
TextConsole& operator << (TextConsole &con, const T &val) {
    std::ostringstream out;
    out << val;

    con.writeString(out.str().c_str());

    return con;
}


#endif
