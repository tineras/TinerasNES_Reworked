/*    PPU.cpp
    Author: Aaron Wiginton

    Description: Handles the PPU operations file for TinerasNES NES emulator.
*/
#include "ppu.h"
#include "cpu.h"
#include "mem.h"

// (FIX) (REMOVE ME) Figure out how to initialize me in the PPU::init function
unsigned char palOffset[4][4] =  {{0x00, 0x00, 0x02, 0x02},
                                  {0x00, 0x00, 0x02, 0x02},
                                  {0x04, 0x04, 0x06, 0x06},
                                  {0x04, 0x04, 0x06, 0x06}};

// Color for Palettes to use
unsigned char palColors[64][4] = {{0x75, 0x75, 0x75, 0x00}, {0x27, 0x1B, 0x8F, 0x00}, {0x00, 0x00, 0xAB, 0x00}, {0x47, 0x00, 0x9F, 0x00}, {0x8F, 0x00, 0x77, 0x00}, {0xAB, 0x00, 0x13, 0x00}, {0xA7, 0x00, 0x00, 0x00}, {0x7F, 0x0B, 0x00, 0x00}, 
                                  {0x43, 0x2F, 0x00, 0x00}, {0x00, 0x47, 0x00, 0x00}, {0x00, 0x51, 0x00, 0x00}, {0x00, 0x3F, 0x17, 0x00}, {0x1B, 0x3F, 0x5F, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, 
                                  {0xBC, 0xBC, 0xBC, 0x00}, {0x00, 0x73, 0xEF, 0x00}, {0x23, 0x3B, 0xEF, 0x00}, {0x83, 0x00, 0xF3, 0x00}, {0xBF, 0x00, 0xBF, 0x00}, {0xE7, 0x00, 0x5B, 0x00}, {0xDB, 0x2B, 0x00, 0x00}, {0xCB, 0x4F, 0x0F, 0x00}, 
                                  {0x8B, 0x73, 0x00, 0x00}, {0x00, 0x97, 0x00, 0x00}, {0x00, 0xAB, 0x00, 0x00}, {0x00, 0x93, 0x3B, 0x00}, {0x00, 0x83, 0x8B, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, 
                                  {0xFF, 0xFF, 0xFF, 0x00}, {0x3F, 0xBF, 0xFF, 0x00}, {0x5F, 0x97, 0xFF, 0x00}, {0xA7, 0x8B, 0xFD, 0x00}, {0xF7, 0x7B, 0xFF, 0x00}, {0xFF, 0x77, 0xB7, 0x00}, {0xFF, 0x77, 0x63, 0x00}, {0xFF, 0x9B, 0x3B, 0x00}, 
                                  {0xF3, 0xBF, 0x3F, 0x00}, {0x83, 0xD3, 0x13, 0x00}, {0x4F, 0xDF, 0x4B, 0x00}, {0x58, 0xF8, 0x98, 0x00}, {0x00, 0xEB, 0xDB, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, 
                                  {0xFF, 0xFF, 0xFF, 0x00}, {0xAB, 0xE7, 0xFF, 0x00}, {0xC7, 0xD7, 0xFF, 0x00}, {0xD7, 0xCB, 0xFF, 0x00}, {0xFF, 0xC7, 0xFF, 0x00}, {0xFF, 0xC7, 0xDB, 0x00}, {0xFF, 0xBF, 0xB3, 0x00}, {0xFF, 0xDB, 0xAB, 0x00}, 
                                  {0xFF, 0xE7, 0xA3, 0x00}, {0xE3, 0xFF, 0xA3, 0x00}, {0xAB, 0xF3, 0xBF, 0x00}, {0xB3, 0xFF, 0xCF, 0x00}, {0x9F, 0xFF, 0xF3, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}};

//    **********
//    PPU Run
#pragma region /* ----- Run PPU ----- */
void PPU::run(int runto)
{
    #pragma region /* Scanline 240 */
    // Idle Scanline - 1 Scanline  (Scanline 240)
    while (scanline == -2)
    {
        while (scanlineCycle < 341)
        {
            tsPPU += 5;
            scanlineCycle++;

            if (tsPPU >= runto)
            { return; }
        }

        scanline = -1;
        scanlineChanged = true;
    }
    #pragma endregion

    #pragma region /* Scanline 241 (Setup) */
    // Trigger events that happen on scanline 241
    if (tsPPU < VBlankTime)
    {
        // Set VBlank to true
        //if(!_tn->reset)
            _mem->setVBlank(true);
        
        // Make sure the system has not just been reset and check to see if Interrupt is enabled on VBLANK
        if((_mem->_memCPU[0x2000] & 0x80) == 0x80)// && !_tn->reset)
        {
            //_tn->currentCPUCycle += _cpu->NMI(); // OLD
            _tn->bumpCurrentCPUCycle(_cpu->NMI());
        }
        if(_cpu->IRQPending)
        {
            //_tn->currentCPUCycle += _cpu->IRQ(); // OLD
            _tn->bumpCurrentCPUCycle(_cpu->IRQ());
            _cpu->IRQPending = false;
        }
        
        // Set Reset to false
        //_tn->reset = false;
        tsPPU += VBlankTime;   // Do nothing during VBlank until the next 'if' statement is false

        // 
        scanline = -1;
        scanlineCycle = 0;
    }
    #pragma endregion

    #pragma region /* VBlank (Scanline 241 + 20 Scanlines) */
    // VBLANK Time - 20 Scanlines  (Scanline 241)
    if (tsPPU >= runto)
    { return; }
    else
    {
        // Clear Sprite Hit, Sprite Overflow and VBLANK at the end of VBlank
        // For reason, clearing Sprite Hit causes the scroll offset to change seemingly randomly
        // So I have disabled the reset of that below
        //if (!_tn->reset)
            _mem->_memCPU[0x2002] &= 0x40;

        // Clear VBlank
        _mem->setVBlank(false);
    }
    #pragma endregion

    #pragma region /* Scanline -1 */
    // PreRender Scanline - 1 Scanline  (Scanline -1)
    while (scanline == -1)
    {
        while (scanlineCycle < 341)
        {
            tsPPU += 5;
            scanlineCycle++;

            if (tsPPU >= runto)
            { return; }
        }

        pixelM = 0;    // Reset Master Pixel
        scanlineCycle = 0;
        scanline++;
        scanlineChanged = true;
    }
    #pragma endregion

    #pragma region /* Scanline 1 - 240 */
    // Render Scanlines - 240 Scanlines  (Scanline 1 - 240)
    while (scanline < 240)
    {
        // Set Rendering TRUE
        //_tn->rendering = true;

        while (scanlineCycle < 256)
        {
            updateDrawLocation();

            renderBackground();
            renderSprites();

            // Increase Master pixel number
            pixelM++;

            tsPPU += 5;
            scanlineCycle++;

            if (tsPPU >= runto)
            { return; }
        }

        while (scanlineCycle < 341)
        {
            tsPPU += 5;
            scanlineCycle++;

            if (tsPPU >= runto)
            { return; }
        }

        scanlineCycle = 0;
        scanline++;
        scanlineChanged = true;
    }
    #pragma endregion

    #pragma region /* Scanline 240 (Cleanup) */
    if (scanline >= 240)
    {
        // Render APU frame
        //tn->apu->renderFrame();

        // Set Rendering to FALSE
        //_tn->rendering = false;

        // Reset PPU Timestamp
        tsPPU = 0;
        readyToRender = true;

        // Reset Scanline to -2
        scanline = -2;
    }
    #pragma endregion
}
#pragma endregion

//    **********
//    Render Background
#pragma region /* ----- Render Background ----- */
void PPU::renderBackground()
{
    #pragma region Draw Single Background Pixel

    // Increment 'X' Tile Scroll after reaching fine X value of 8
    if (fX >= 0x08)
    {
        // Reset fine X
        fX = 0;

        // If scroll X reaches its limis xxxxxxxxx11111, reset it 
        if ((scrollXY_CurrentTileNum & 0x001F) == 0x001F)
        {
            // If end of X scroll, change to next nametable and continue drawing
            nametableNum ^= 0x01;

            // Reset X scroll
            scrollXY_CurrentTileNum &= ~0x001F;
        }
        else
        {
            // Increment X scroll
            scrollXY_CurrentTileNum++;
            //bolNewTile = true;
        }
    }

    // Check to see if BG is turned on/off
    if ((_mem->_memCPU[0x2001] & 0x08) == 0x08)
    { bolDrawBG = true; }
    else
    { bolDrawBG = false; }

    if (bolDrawBG)
    {
        // Get the Palette number to use for the current pixel
        int pal = getPal();

        // Get Nametable Byte
        unsigned char nametableOffset = _mem->getPPU(nametable + BGTile);

        // Get the Palette Color to use for the current pixel
        unsigned char BGColor = getColor(nametableOffset);

        // Combine pal and color offset to get final color of current pixel (only 64 (0x3F) possible colors)
        unsigned char finalPixelColor = _mem->getPPU(pal + BGColor) & 0x3F;

        // Reset Alpha and then Get BGAlpha
        unsigned char BGAlpha = 0xFF;
        bgAlpha = true;
        if (BGColor == 0)
        {
            BGAlpha = 0x00;        // Set current pixel Alpha to '0x00' (transparent)
            bgAlpha = false;    // To easily let sprites know if BG pixel is transparent '0x00' = false, '0xFF' = true
        }
        else if(BGColor != 0)
        {
            BGAlpha = 0xFF;
        }

        // Get data for drawing tiles without the scroll offsets
        int scanlineTemp = pixelM / 256;
        int bgTileLineNumberTemp = scanlineTemp % 8;
        int pixelXTemp = pixelM % 256;
        int bgPixelNumberTemp = pixelXTemp % 8;
        int bgTileNumberTemp = (pixelXTemp / 8) + ((scanlineTemp / 8) * 32);

        // Draw R,G,B,A of Current Background Pixel        
        drawBG(bgTileNumberTemp, bgTileLineNumberTemp, bgPixelNumberTemp, palColors[finalPixelColor][0], 2);
        drawBG(bgTileNumberTemp, bgTileLineNumberTemp, bgPixelNumberTemp, palColors[finalPixelColor][1], 1);
        drawBG(bgTileNumberTemp, bgTileLineNumberTemp, bgPixelNumberTemp, palColors[finalPixelColor][2], 0);
        drawBG(bgTileNumberTemp, bgTileLineNumberTemp, bgPixelNumberTemp, BGAlpha, 3);
    }
    #pragma endregion
}
#pragma endregion

//    **********
//    Render Sprites
#pragma region /* ----- Render Sprites ----- */
void PPU::renderSprites()
{
    #pragma region Draw Sprites to Frame Buffer

    // Check to see if Sprites are turned on/off
    if ((_mem->_memCPU[0x2001] & 0x10) == 0x10)
    { bolDrawSP = true; }
    else
    { bolDrawSP = false; }

    if (bolDrawSP)
    {
        pixelX = (pixelM % 256);

        if (scanlineChanged)
        {
            // Reset scanlineChanged so it does not enter 'if' unless new scanline
            scanlineChanged = false;

            // Check each sprite in OAM until you find a good one (64 Sprites * 4 Attribute Bytes)
            checkScanlineForSprites();
        }

        if (spritesFound)
        {
            if (getSpriteToDraw())
            {

                // Get sprite palette
                int SPPal = 0x3F10 + (_mem->_memSPRRAM[spriteToDraw[spriteIndex] + 2] & 0x03) * 4;

                // ???
                //int pixel = pixelX - _mem->memSPRRAM[spriteToDraw[spriteIndex] + 3];

                // Check for 0 Sprite Hit
                if (bgAlpha && spriteToDraw[spriteIndex] == 0)
                {
                    _mem->_memCPU[0x2002] |= 0x40;
                }

                // If (not transparent && not behing BG) || BG alpha is 0
                if (SPColor != 0 && ((_mem->_memSPRRAM[spriteToDraw[spriteIndex] + 2] & 0x20) == 0x00) || !bgAlpha)
                {
                    // Just here to make the drawing location easier to read
                    int y = scanline;
                    int x = pixelX;

                    int intPixelColor = _mem->getPPU(SPPal + SPColor) & 0x3F;

                    drawSP(x, y, palColors[intPixelColor][2], 0);
                    drawSP(x, y, palColors[intPixelColor][1], 1);
                    drawSP(x, y, palColors[intPixelColor][0], 2);
                    drawSP(x, y, SPAlpha, 3);
                }
            }
        }
    }
    #pragma endregion
}
#pragma endregion

//    **********
//    Sprites - Check *Next* Scanline For Sprites
#pragma region /* ----- Sprites - Check *Next* Scanline For Sprites ----- */
void PPU::checkScanlineForSprites()
{
    // Update all previously retreived sprite values as the new current values
    spritesFound = spritesFoundT;           // Stores the location of each sprite found
    //spriteToDraw = spriteToDrawT;           // Number of sprites found
    for (int c = 0; c < sizeof(spriteToDraw); c++) { spriteToDraw[c] = spriteToDrawT[c]; }    // Number of sprites found
    spriteIndex = spriteIndexT;             // Any sprites found?
    _mem->_memCPU[0x2002] &= memCPU2002T;  // Reset '8 sprites on a scanline' bit

    // Reset Temp Sprite Values for next evaluation
    memset(spriteToDrawT, 0, sizeof(spriteToDrawT));    // Stores the location of each sprite found
    spriteIndexT = 0x00;            // Number of sprites found
    spritesFoundT = false;          // Any sprites found?
    memCPU2002T = 0xDF;             // Reset '8 sprites on a scanline' bit

    // Check each sprite in OAM until you find a good one (64 Sprites * 4 Attribute Bytes)
    for (int i = 0; i < 0xFF; i += 4)
    {
        // Set default sprite height
        int spriteHeight = ((_mem->_memCPU[0x2000] & 0x20) == 0x20) ? 16 : 8;

        // Check to see if the sprite is on the current scanline in the Y direction
        if (scanline - (unsigned char)(_mem->_memSPRRAM[i]) < spriteHeight &&    // Is the sprite within 8 or 16 pixels of current scanline
            scanline - (unsigned char)(_mem->_memSPRRAM[i]) >= 0 &&              // ...
            spriteIndexT < 8 &&         // Less than 8 sprites found?
            _mem->_memSPRRAM[i] > 0 && // Is the sprite on screen between the 0 and EF scanline?
            _mem->_memSPRRAM[i] < 0xEF) // ...
        {
            // Check to see if the sprite is on the line in the X direction within the screen limits
            if (_mem->_memSPRRAM[i + 3] >= 0 && _mem->_memSPRRAM[i + 3] <= 255)
            {
                spriteToDrawT[spriteIndexT++] = (unsigned char)i;

                spritesFoundT = true;

                if (spriteIndexT >= 8)
                {
                    // Set '8 sprites on a scanline' bit
                    memCPU2002T = 0xFF;

                    // Get out of loop and stop checking for sprites to be drawn. Go to next scanline.
                    i = 64 * 4;
                }
            }
        }
    }

    if(spritesFound)
    {
        // Get the total number of sprite for each scanline
        numSprites = (unsigned char)(spriteIndex - 1);

        // (FIX) (REMOVE) This seems unnecessary...but may not be
        // Reset sprite index to 0; 
        spriteIndex = 0;
    }
}
#pragma endregion

//    **********
//    Sprites - Get Index of Current Sprite to Draw
#pragma region /* ----- Sprites - Get Index of Current Sprite to Draw ----- */
bool PPU::getSpriteToDraw()
{
    for (int i = 0; i <= numSprites; i++ )
    {
        // Get X Position of Sprite
        int sprPixel = _mem->_memSPRRAM[spriteToDraw[i] + 3];

        // Compare X Position of Sprite with Current Location of pixelX that is being drawn
        if (pixelX - sprPixel < 8 && pixelX - sprPixel >= 0)
        {
            // Check to see if the pixel of the sprite is transparent
            if (checkTransparency(i))
            {
                // If not transparent, set the index of the sprite just found so it will be drawn
                spriteIndex = i;
                return true;
            }
        }
    }

    // If sprite is not within current pixelX position && it is Transparent, do nothing
    return false;
}
#pragma endregion

//    **********
//    Sprites - Check Sprite Transparency
#pragma region /* ----- Sprites - Check Sprite Transparency ----- */
bool PPU::checkTransparency(int i)
{
    // NO NEED TO CHECK FOR spLineNumber, pal, Pattern Table, flipping on every pixel .... only every line except for flipping which is every tile (and mybe others too)
    int spLineNumber = (unsigned char)(scanline - (unsigned char)(_mem->_memSPRRAM[spriteToDraw[i]] + 1));

    // Get the current line number that sprites are being drawn on
    int line = spLineNumber;

    // Check for sprite flipping
    bool flipY = ((_mem->_memSPRRAM[spriteToDraw[i] + 2] & 0x80) == 0x80);
    bool flipX = ((_mem->_memSPRRAM[spriteToDraw[i] + 2] & 0x40) == 0x40);

    // Set flipping
    int pixel = pixelX - _mem->_memSPRRAM[spriteToDraw[i] + 3];
    int temp = 7 - pixel;
    if (flipX)
    {
        temp = pixel;       //  THIS IS MESSED UP
        //x = x + (7 - pixel) * 2;
    }

    // #### Get bytes from SPRRAM ####
    #pragma region Get OAM Pattern Table Bytes
    
    // If the sprites are 8x8
    if ((_mem->_memCPU[0x2000] & 0x20) == 0x00) // for 8x8 sprites
    {
        if (flipY)
        {
            line = (unsigned char)(7 - spLineNumber);
        }

        // Get location of the sprites (Pattern Table 0x0000 or 0x1000)
        int spritePatternTableAddr = ((_mem->_memCPU[0x2000] & 0x08) >> 3) * 0x1000;

        unsigned char PTable2 = _mem->getPPU(spritePatternTableAddr + _mem->_memSPRRAM[spriteToDraw[i] + 1] * 16 + line);  // Get sprite data piece 1 to combine with data piece 2 to get color of pixel
        unsigned char PTable2a = _mem->getPPU(spritePatternTableAddr + _mem->_memSPRRAM[spriteToDraw[i] + 1] * 16 + line + 8);  // Get sprite data piece 2 to combine with data piece 1 to get color of pixel

        // Combine data and get color of pixel
        SPColor = (unsigned char)(((PTable2 >> temp) & 0x01) | (((PTable2a >> temp) & 0x01) * 2)); 
    }
    // Else If the sprites are 8x16
    else if ((_mem->_memCPU[0x2000] & 0x20) == 0x20) // for 8x16 sprites
    {
        // Check to see if there is a 0x1000 offset
        int intOffset = (_mem->_memSPRRAM[spriteToDraw[i] + 1] & 0x01) * 0x1000;

        // Get tile number of the specific sprite
        int byteTemp = (unsigned char)(_mem->_memSPRRAM[spriteToDraw[i] + 1] & 0xFE);

        // Looks at the top half of the sprite
        if (spLineNumber < 8)
        {
            if (flipY)
            {
                line = (unsigned char)(7 - spLineNumber) + 16;
            }

            unsigned char PTable2 = _mem->getPPU(byteTemp * 16 + intOffset + line);  // Get sprite data piece 1 to combine with data piece 2 to get color of pixel
            unsigned char PTable2a = _mem->getPPU(byteTemp * 16 + intOffset + line + 8);  // Get sprite data piece 2 to combine with data piece 1 to get color of pixel
            
            // Combine data and get color of pixel
            SPColor = (unsigned char)(((PTable2 >> temp) & 0x01) | (((PTable2a >> temp) & 0x01) * 2));
        }

        else if (spLineNumber >= 8)
        {
            if (flipY)
            {
                line = (unsigned char)(15 - spLineNumber) - 8;
            }

            unsigned char PTable2b = _mem->getPPU(byteTemp * 16 + intOffset + line + 8);  // Get sprite data piece 1 to combine with data piece 2 to get color of pixel
            unsigned char PTable2c = _mem->getPPU(byteTemp * 16 + intOffset + line + 8 + 8);  // Get sprite data piece 2 to combine with data piece 1 to get color of pixel
            
            // Combine data and get color of pixel
            SPColor = (unsigned char)(((PTable2b >> temp) & 0x01) | (((PTable2c >> temp) & 0x01) * 2));
        }
    }
    #pragma endregion

    // Set transparency to off
    SPAlpha = 0xFF;

    if (SPColor == 0x00)
    {
        return false;
    }
    else
    {
        return true;
    }
}
#pragma endregion

//    **********
//    Update Draw Location ('t' operations)
#pragma region /* ----- Update Draw Location ('t' operations) ----- */
void PPU::updateDrawLocation()
{
    // Update 'v' at Beginning of frame
    if (pixelM == 0)
    {
        v = _mem->t;
        
        //scrollYFine = (byte)((v & 0x7000) >> 12);
        
        nametableNum = (v >> 10) & 0x03;
        scrollXY_CurrentTileNum = (v & 0x03FF);
        nametable = 0x2000 +(nametableNum * 0x0400);
        
        scanlineChanged = true;

        // bolNewTile = true;
    }

    // Update 'v' at Beginning of each scanline
    else if (pixelM % 256 == 0)
    {
        // END OF SCANLINE
        if ((v & 0x7000) == 0x7000)
        {
            v &= ~0x7000;

            if ((v & 0x03E0) == 0x3A0)
            { v ^= 0x800 | 0x3A0; }
            else if ((v & 0x03E0) == 0x3E0)
            { v ^= 0x3E0; }
            else
            { v += 0x0020; }
        }
        else
        {
            v += 0x1000;
        }

        // Look for changes in fineX to update it below
        fXTemp = _mem->fineX;
        fX = _mem->fineX;

        // BEGINNING OF SCANLINE
        int temp = _mem->t & 0x041F; //041f
        //int temp = t & 0x041F; //041f
        v &= ~0x041F;
        v |= temp;

        // Set X Offset
        //scrollX = (byte)(v & 0x001F); //<<<< --- THIS LINE CAUSES A PROBLEM WHEN CHANGING t to v.  THIS IS LIKELY BECAUSE OF 2006 WRITES HAPPENING THAT ARE NOT RELATED TO SCROLLING
        scrollXY_CurrentTileNum = (v & 0x03FF);

        // Get NameTable
        nametableNum = (char)((v & 0x0C00) >> 10);
        nametable = 0x2000 + (nametableNum * 0x0400);

        //    bolNewTile = true;
    }

    // FineX update
    if (fXTemp != _mem->fineX)
    {
        fX = _mem->fineX;
    }

    // Update nametable before checking masking
    nametable = 0x2000 +(nametableNum * 0x0400);
    ///********************************* NEED TO FIX THIS MEMORY MASKING STUFF ****************************/
    nametable = _mem->getMaskedAddrData(nametable);
    ///********************************* NEED TO FIX THIS MEMORY MASKING STUFF ****************************/

    BGTile = scrollXY_CurrentTileNum;
    BGTileLine = (v & 0x7000) >> 12;
    BGPixel = fX++;

    // Set Background Color
    if (pixelM == (256 * 240 - 1))
    {
        // Set backgound color ONLY
        unsigned char bTemp = (unsigned char)(_mem->getPPU(0x3F00) & 0x3F);
        BGColorR = palColors[bTemp][0]; // R
        BGColorG = palColors[bTemp][1]; // G
        BGColorB = palColors[bTemp][2]; // B
        BGColorA = 0xFF;                // A

        if(BGColorR == prevBGColorR &&
           BGColorG == prevBGColorG &&
           BGColorB == prevBGColorB &&
           BGColorA == prevBGColorA)
        {
            BGChanged = false;
        }
        else
            BGChanged = true;

        prevBGColorR = BGColorR;
        prevBGColorG = BGColorG;
        prevBGColorB = BGColorB;
        prevBGColorA = BGColorA;
        


    //    // Counter for drawing the 4 nametable for debugging - Only draw ever N frames                
    //    if (NTDebugOffset++ >= 30)
    //    {
    //        NTDebugOffset = 0;
    //    }
    }
}
#pragma endregion

//    **********
//    Draw Background
#pragma region /* ----- Draw Background ----- */
void PPU::drawBG(int tile, int line, int pixel, unsigned char color, unsigned char rgbOffset)
{
    framePreBuffer[(tile * 32 + line * 1024 + (pixel) * 4) + (int)(tile / 32) * 32 * 224 + rgbOffset] = color;
}
#pragma endregion

//    **********
//    Draw Sprites
#pragma region /* ----- Draw Sprites ----- */
void PPU::drawSP(int x, int y, unsigned char color, unsigned char rgbOffset)
{
    if (y < 240)
    {
        framePreBuffer[x * 4 + y * 1024 + rgbOffset] = color;
    }
}
#pragma endregion

//    **********
//    Get Palette Number
#pragma region /* ----- Get Palette Number ----- */
int PPU::getPal()
{
    #pragma region // Get Palette for each tile

    //    1. Get the Tile Row and Column (of possible 32 rows and 32 columns)
    //    2. Subdivide the 32 possible rows and colums into their 8x8(64)
            // total palette blocks (4x4(16) tiles in each block). Each block is represented by a single byte.
            // This byte 33221100 ('attr') is used to determine which palette number to use [0-3]
    //    3. Get 'attr' byte
    //    4. Set 'pal' to the specific Palette to use for the current pixel
    //  NOTE: bmrow and bmcol are used to figure out how many bits to shift 'attr' by in order to get the correct
            // 2 palette bits from 'attr'

    // Get tile Row (0-29) and Column (0-31)
    row = BGTile / 32;
    col = BGTile % 32;

    // Get Block Row (0-7) and Column (0-7)
    brow = row / 4;
    bcol = col / 4;

    // Get modulus within each Block's Row (0-3) and Column (0-3)
    bmrow = row % 4;
    bmcol = col % 4;

    // Get the attribute byte associated with the tile using the Block Row and Block Column
    unsigned char attr = _mem->getPPU(nametable + 0x03C0 + (8 * brow + bcol));

    // Get the Palette to use by shifting a certain number of bits found in the lookup table
    return 0x3F00 + ((attr >> (palOffset[bmrow][bmcol])) & 0x03) * 4;
    #pragma endregion
}
#pragma endregion

//    **********
//    Get Palette Color
#pragma region /* ----- Get Palette Color ----- */
unsigned char PPU::getColor(unsigned char ntOffset)
{
    #pragma region // Get color for each pixel
    //    1. Get location of the Attribute Byte that the Nametable is pointing to
    //    2. Get First Pattern Table byte
    //    3. Get Second Pattern Table byte
    //    4. 'OR' the two bytes to get the offset that will be added to the pal number to get the color
    //    5. Determine if the pixel is transparent (is palColor '0'?) and set Alpha
    //  6. Add palOffset(color) to pal(palette number) to get final color (ex. 3F01 + 2 = 3F03)
    //  7. 'AND' result if 0x3F because there are only 64 possible colors
    //  8. Get final color from the predefined color table (palColors) <<-- This is done at the same time as step 9.
    //  9. Draw the R,G,B,A of each pixel by passing in the [Tile, Line, Pixel and Color] of each pixel

    // Saving cycles
    int intPtrLoc = (((_mem->_memCPU[0x2000] & 0x10) >> 4) * 0x1000) + ntOffset * 16 + BGTileLine;

    unsigned char PTable1 = _mem->getPPU(intPtrLoc);  // Get background pattern table byte 1 of 2 for combining
    unsigned char PTable2 = _mem->getPPU(intPtrLoc + 0x08);  // Get background pattern table byte 2 of 2 for combining

    // Add high and low pattern table bytes to get corresponding color for each pixel
    return (unsigned char)(((PTable1 >> (7 - BGPixel)) & 0x01) | (((PTable2 >> (7 - BGPixel)) & 0x01) * 2)); // Get color
    #pragma endregion
}
#pragma endregion

//    **********
//    PPU Init Variables
#pragma region /* ----- PPU Init Variables ----- */
void PPU::init(CPU* cpu, MEM* mem)
{
    _cpu = cpu;
    _mem = mem;

    reset();
}
#pragma endregion

//    **********
//    Reset PPU Variables
#pragma region /* ----- Reset PPU Variables ----- */
void PPU::reset()
{
    // PPU TimeStamp
    tsPPU = 0;

    // Init Variables
    VBlankTime = 20 * 341 * 5; // 20 Scanlines, 340 Pixels Per Scanline, 5 PPU Timestamp Cycles

    // Clear Framebuffer
    memset(framePreBuffer, 0, sizeof(framePreBuffer));

    // Set bolDrawBG and bolDrawSP to 'True' so that the BG and Srites are being drawn be default
    bolDrawBG = true;
    bolDrawSP = true;

    // Set BG and Sprite Clipping
    bolClipBG = true;
    bolClipSP = true;

    // Sprite Related Variables
    spritesFound = false;    // Have sprites been found on current scanline?
    memset(spriteToDraw, 0, sizeof(spriteToDraw));        // (spriteToDraw[8]) Stores the location of each sprite that has been found
    spriteIndex = 0;        // Number of sprites found on current scanline...this is reassigned, that's why numSprites exists as well
    spritesFoundT = false;    // (Cached value from previous scanline read) Have sprites been found on current scanline?
    memset(spriteToDrawT, 0, sizeof(spriteToDrawT));    // (spriteToDrawT[8]) (Cached value from previous scanline read) Stores the location of each sprite that has been found
    spriteIndexT = 0;        // (Cached value from previous scanline read) Number of sprites found on current scanline
    memCPU2002T = 0xDF;        // Used to Reset '8 sprites on a scanline' bit
    numSprites = 0;            // Number of sprites found on current scanline
    pixelX = 0;                // X position of the current pixel being evaluated
    SPColor = 0;            // Stores result of Pattern Table Color Fetch

    BGColorR = 0x00; // R
    BGColorG = 0x00; // G
    BGColorB = 0x00; // B
    BGColorA = 0xFF; // A

    prevBGColorR = 0x00; // R
    prevBGColorG = 0x00; // G
    prevBGColorB = 0x00; // B
    prevBGColorA = 0xFF; // A

    BGChanged = true;

    // Set Background Alpha
    bgAlpha = true;    // To easily let sprites know if BG pixel is transparent '0x00' = false, '0xFF' = true
    spAlpha = true;
    SPAlpha = 0xFF;

    row = 0; col = 0; brow = 0; bcol = 0; bmrow = 0; bmcol = 0;

    v = 0;
    scrollXY_CurrentTileNum = 0;
    fX = 0;
    fXTemp = 0;

    nametable = 0;
    nametableNum = 0;

    BGTile= 0;        // Current Background Tile Number
    BGTileLine = 0;    // Current Background Line Number withing Current BGTile
    BGPixel = 0;    // Current Background Pixel Number within Current BGTile

    // Clear Master Pixel
    pixelM = 0;

    nametable = 0x2000;
}
#pragma endregion

//    **********
//    PPU Constuctor
#pragma region /* ----- PPU Constructor ----- */
PPU::PPU(TinerasNES* tn) : readyToRender(false)
{
    _tn = tn;
}
#pragma endregion

//    **********
//    PPU Destructor
#pragma region /* ----- PPU Destructor ----- */
PPU::~PPU()
{

}
#pragma endregion
