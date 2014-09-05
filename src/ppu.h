#ifndef _PPU_HEADER
#define _PPU_HEADER

#include "TinerasNES.h"

//using namespace std;

class TinerasNES;
class CPU;
class MEM;

class PPU
{
    /******  *****  REGISTERS ***** ******/
public:    
    /* Declaration for Constructor    */
    PPU(TinerasNES* tn);
    ~PPU();

    // Init PPU Variables
    void init(CPU* cpu, MEM* mem);

    // Draw Background/Sprites to frame pre-buffer
    void drawBG(int tile, int line, int pixel, unsigned char color, unsigned char rgbOffset);    // Render Background to frame pre-buffer
    void drawSP(int x, int y, unsigned char color, unsigned char rgbOffset);    // Render Sprites to frame pre-buffer

    bool readyToRender() { return _ready_to_render; }
    void setReadyToRender(bool ready_to_render) { _ready_to_render = ready_to_render; }

    // Run PPU
    void run(int);

    // Update 'V' drawing locations
    void updateDrawLocation();

    // Reset PPU Variables
    void reset();

    // Background RGBA
    unsigned char BGColorR; // R
    unsigned char BGColorG; // G
    unsigned char BGColorB; // B
    unsigned char BGColorA; // A

    unsigned char prevBGColorR; // R
    unsigned char prevBGColorG; // G
    unsigned char prevBGColorB; // B
    unsigned char prevBGColorA; // A

    bool BGChanged;

    // Main draw buffer
    unsigned char framePreBuffer[256 * 240 * 4];    // Final rendered frame to draw. Contains BG and Sprites.

    bool bolDrawBG;
    bool bolDrawSP;
    bool bolClipBG;
    bool bolClipSP;

private:
    // Render Background/Sprites
    void renderBackground();
    void renderSprites();

    // Sprite Functions
    void checkScanlineForSprites();
    bool getSpriteToDraw();
    bool checkTransparency(int);

    // Get Palette Number for Background
    int getPal();
    unsigned char getColor(unsigned char);

    // Draw each pixel
    void drawPixel();

    // Run to 'VBlankTime' 
    int VBlankTime; // 20 Scanlines, 340 Pixels Per Scanline, 5 PPU Timestamp Cycles
    bool _ready_to_render; // Ready to draw to screen?

    int tsPPU;    // PPU Timestamp
    int scanline;    // Scanline number
    int  scanlineCycle;    // Scanline Cycle
    bool scanlineChanged; // Has the scanline number changed

    // Palette - Used to get the palette of the current pixel
    int row, col, brow, bcol, bmrow, bmcol;
    bool bgAlpha;
    bool spAlpha;
    unsigned char SPAlpha;
    //char palOffset[][]; // (FIX) Figure out how to initialize me in the PPU::init function

    int v, fX, fXTemp;
    int scrollXY_CurrentTileNum; // Gives current tile number by telling by how many X and Y tiles the current scroll is set to.

    // Sprite Related Variables
    bool spritesFound;                // Have sprites been found on current scanline?
    unsigned char spriteToDraw[8];    // Stores the location of each sprite that has been found
    unsigned char spriteIndex;        // Number of sprites found on current scanline...this is reassigned, that's why numSprites exists as well
    bool spritesFoundT;                // (Cached value from previous scanline read) Have sprites been found on current scanline?
    unsigned char spriteToDrawT[8];    // (Cached value from previous scanline read) Stores the location of each sprite that has been found
    unsigned char spriteIndexT;        // (Cached value from previous scanline read) Number of sprites found on current scanline
    unsigned char memCPU2002T;        // Used to Reset '8 sprites on a scanline' bit
    int numSprites;                    // Number of sprites found on current scanline
    unsigned char SPColor;            // Stores result of Pattern Table Color Fetch
    
    int pixelM;    // Master Pixel Number 0 - 61439
    int pixelX;    // X position of the current pixel being evaluated

    int nametable;        // Current nametable (0x2000, 0x2400, 0x2800, 0x2C00)
    int nametableNum;    // Tell which nametable number the program is current on (0-3)

    int BGTile;        // Current Background Tile Number
    int BGTileLine;    // Current Background Line Number withing Current BGTile
    int BGPixel;    // Current Background Pixel Number within Current BGTile

    // Pointer to TinerasNES object (set in constructor)
    TinerasNES *_tn;
    MEM* _mem;
    CPU* _cpu;
};
#endif// _CPU_HEADER