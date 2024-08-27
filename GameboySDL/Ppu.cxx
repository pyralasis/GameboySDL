#include "Ppu.hxx"
#include "Memory.hxx"
#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <stack>
#include <sstream>
#include <string>

bool spriteSorter(Sprite const& left, Sprite const& right)
{
    return left.xPos < right.xPos;
}

Ppu::Ppu()
{
}

Ppu::~Ppu()
{
}

bool Ppu::getIsFrameReady()
{
    return isFrameReady;
}

void Ppu::setIsFrameReady(bool isReady)
{
    isFrameReady = isReady;
}

void Ppu::cycle()
{
    if (mode == 2)
    {
        modeTwo();
    }
    else if (mode == 3)
    {
        modeThree();
    }
    else if (mode == 0)
    {
        modeZero();
    }
    else if (mode == 1)
    {
        modeOne();
    }
}

void Ppu::setMemory(Memory *value)
{
    memory = value;
}

void Ppu::modeZero()
{
    currentScanlineTicks++;
    if (currentScanlineTicks == 456 && memory->readMemory(0xFF44) < 143)
    {
        mode = 2;
        currentScanlineTicks = 0;
        memory->writeMemory(0xFF44, memory->readMemory(0xFF44) + 1);
        currentOamAddress = 0xFE00;
        oamFifo = {};
        backgroundFifo = {};
        backgroundFetchStep = 1;
        pixelsPushed = 0;
        viewportXTile = 0;
        if (renderWindow)
        {
            windowXOffset = 0;
            windowYOffset++;
        }
        spriteXOffset = 0;
        oamBuffer.clear();
    }
    else if (currentScanlineTicks == 456 && memory->readMemory(0xFF44) == 143)
    {
        mode = 1;
        currentScanlineTicks = 0;
        memory->writeMemory(0xFF44, memory->readMemory(0xFF44) + 1);
        memory->writeMemory(0xFF0F, memory->readMemory(0xFF0F) | 1);
        currentOamAddress = 0xFE00;
        oamFifo = {};
        backgroundFifo = {};
        backgroundFetchStep = 1;
        pixelsPushed = 0;
        viewportXTile = 0;
        windowXOffset = 0;
        windowYOffset = 0;
        spriteXOffset = 0;
        oamBuffer.clear();
    }
}

// Vblank Period. Scanlines 144-153
void Ppu::modeOne()
{
    const unsigned_four_byte ioStart = 0xFF00;

    currentScanlineTicks += 2;
    if (currentScanlineTicks == 456 && memory->readMemory(ioStart + 0x44) < 153)
    {
        currentScanlineTicks = 0;
        memory->writeMemory(ioStart + 0x44, memory->readMemory(ioStart + 0x44) + 1);
    }
    else if (currentScanlineTicks == 456 && memory->readMemory(ioStart + 0x44) == 153)
    {
        currentScanlineTicks = 0;
        mode = 2;
        memory->writeMemory(ioStart + 0x44, 0);
        setIsFrameReady(true);
        renderWindow = false;
        currentOamAddress = 0xFE00;
    }
}

void Ppu::modeTwo()
{
    oamScan();
    currentScanlineTicks += 2;
    if (currentScanlineTicks == 80)
    {
        int fetcherXPos = 0;
        mode = 3;
        int backgroundPixelsRendered = 0;
        backgroundFifo = {};
        oamFifo = {};
        std::sort(oamBuffer.begin(), oamBuffer.end(), &spriteSorter);
    }
}

void Ppu::modeThree()
{
    const unsigned_four_byte ioStart = 0xFF00;

    // Check if the window should be displayed instead of the background for the rest of the scanline
    if (!renderWindow &&
        (memory->readMemory(ioStart + 0x40) & 0x20) >> 5 == 1 &&                    // LCDC window Enabled Bit
        memory->readMemory(ioStart + 0x4A) == memory->readMemory(ioStart + 0x44) && // curruent scanline is
        pixelsPushed >= (memory->readMemory(ioStart + 0x4B) - 7))
    {
        renderWindow = true;
        backgroundFetchStep = 1;
        backgroundFifo = {};
    }
    int ticksPassed = 0;
    if (!isFetchingSprite)
    {
        //If there are sprites and the oamFifo is empty, check to see if a sprite should be drawn in the current cycle
        if (oamFifo.size() == 0)
        {
            int i = 0;
            while (i < oamBuffer.size() && !isFetchingSprite) {
                int spriteX = oamBuffer[i].xPos;
                int pixelX = (((viewportXTile - 1) * 8) + (8 - backgroundFifo.size()));
                if (spriteX <= pixelsPushed + 8) {

                    backgroundFetchStep = 1;
                    if (oamBuffer[i].attributes >> 7) {
                        backgroundPriority = true;
                    }
                    else {
                        backgroundPriority = false;
                    }
                    currentSprite = oamBuffer[i];
                    oamBuffer.erase(oamBuffer.begin() + i);
                    isFetchingSprite = true;
                }
                i++;
            }
        }
    }
    else
    {
        ticksPassed = spriteFetch();
    }
    //Fill the background Fifo using the fetch process
    ticksPassed = backgroundFetch();

    // Push to viewport
    for (int i = 0; i < ticksPassed; i++)
    {
        if (!isFetchingSprite && !backgroundFifo.empty() && pixelsPushed < 160)
        {
            // push background no sprite
            if (oamFifo.empty())
            {
                viewport.push_back(static_cast<int>(backgroundFifo.front()));
                backgroundFifo.pop();
                pixelsPushed++;
            }
            else if (!oamFifo.empty())
            {
                // if sprite pixel is transparent
                if (oamFifo.front() == 255)
                {
                    viewport.push_back(static_cast<int>(backgroundFifo.front()));
                    backgroundFifo.pop();
                    oamFifo.pop();
                    pixelsPushed++;
                }
                //Background has priority and background pixel is not 0
                else if (backgroundPriority && backgroundFifo.front() != 0)
                {
                    viewport.push_back(static_cast<int>(backgroundFifo.front()));
                    backgroundFifo.pop();
                    oamFifo.pop();
                    pixelsPushed++;
                }
                else
                {
                    viewport.push_back(static_cast<int>(oamFifo.front()));
                    backgroundFifo.pop();
                    oamFifo.pop();
                    pixelsPushed++;
                }
            }
            if(backgroundFifo.empty())
            {
                spriteXOffset++;
            }
        }
        if (pixelsPushed >= 160)
        {
            mode = 0;
            i = ticksPassed;
        }
    }
    currentScanlineTicks += ticksPassed;
}

int Ppu::backgroundFetch()
{
    const unsigned_four_byte ioStart = 0xFF00;
    int ticksPassed = 0;
    switch (backgroundFetchStep)
    {
        // Get Tile
    case 1:
    {
        unsigned_two_byte scy = memory->readMemory(ioStart + 0x42); // display screen top left Y coordinate
        unsigned_two_byte scx = memory->readMemory(ioStart + 0x43); // display screen top left X coordinate

        unsigned_two_byte wy = memory->readMemory(ioStart + 0x4A); // Window top left Y coordinate
        unsigned_two_byte wx = memory->readMemory(ioStart + 0x4B); // Window top left X coordinate

        unsigned_two_byte lcdc = memory->readMemory(ioStart + 0x40); // LCD Control
        unsigned_two_byte ly = memory->readMemory(ioStart + 0x44);

        // Check which tile map you should use
        unsigned_four_byte tileMapAddress = 0x9800;
        if ((((lcdc & 0x4) >> 3) == 1 && viewportXTile < wx - 7) || (((lcdc & 0x40) >> 4) == 1 && viewportXTile >= wx - 7))
        {
            tileMapAddress = 0x9C00;
        }

        // Check which region of memory should be used for tile data
        unsigned_four_byte tileDataBase = 0x9000;
        if ((lcdc & 0x16) >> 4)
        {
            tileDataBase = 0x8000;
        }

        int xOffset, yOffset;
        unsigned_two_byte tileNumber;
        unsigned_four_byte tileAddressBase;
        if (!renderWindow) // background
        {
            xOffset = ((scx / 8) + (viewportXTile)) & 0x1F;
            yOffset = ((scy + ly) & 0xFF);
            tileNumber = memory->readMemory(tileMapAddress + xOffset + (32 * (yOffset / 8)));
            tileAddressBase = tileDataBase + 16 * (tileDataBase == 0x8000 ? tileNumber : static_cast<signed_two_byte>(tileNumber));
            tileFetchAddress = tileAddressBase + (2 * (yOffset % 8));

        }
        else // window
        {
            if ((lcdc & 0x40) >> 6) {
                tileMapAddress = 0x9C00;
            }
            tileNumber = memory->readMemory(tileMapAddress + windowXOffset + (32 * (windowYOffset / 8)));
            tileAddressBase = tileDataBase + 16 * (tileDataBase == 0x8000 ? tileNumber : static_cast<signed_two_byte>(tileNumber));
            tileFetchAddress = tileAddressBase + (2 * (windowYOffset % 8));
        }


        backgroundFetchStep = 2;
        ticksPassed = 2;
        break;
    }
    // Get Tile Data Low
    case 2:
    {
        tileFetchLow = memory->readMemory(tileFetchAddress);
        backgroundFetchStep = 3;
        ticksPassed = 2;
        break;
    }
    // Get Tile Data High
    case 3:
    {
        tileFetchHigh = memory->readMemory(tileFetchAddress + 1);
        backgroundFetchStep = 4;
        ticksPassed = 2;
        break;
    }
    // Sleep
    case 4:
    {
        backgroundFetchStep = 5;
        ticksPassed = 2;
        break;
    }
    // Push
    case 5:
    {
        if (backgroundFifo.empty() && pixelsPushed < 160)
        {
            backgroundFifo = decodeTileFetchData(tileFetchLow, tileFetchHigh, memory->readMemory(0xFF47));

            if (pixelsPushed == 0 && !renderWindow) // Account for subtile scrolling by discarding pixels from the first tile that are not in the viewport.
            {
                unsigned_two_byte scx = memory->readMemory(0xFF43);
                for (int i = 0; i < scx % 8; i++)
                {
                    backgroundFifo.pop();
                }
            }
            backgroundFetchStep = 1;
            viewportXTile++; // increment viewport x tile offset.
            
        }
           //If the window is being rendered, increment the offset to track which window tile we are on.
        if (renderWindow)
        {
            windowXOffset++;

        }

        ticksPassed = 1;
        break;
    }
    }
    return ticksPassed;
}

int Ppu::spriteFetch()
{
    int ticksPassed = 0;
    switch (oamFetchStep)
    {
    case 1:
    {
        unsigned_two_byte ly = memory->readMemory(0xFF44);
        unsigned_four_byte address = 0x8000 + (16 * currentSprite.tileIndex);
        spriteFetchAddress = address + (2 * (ly - (currentSprite.yPos - 16)));
        oamFetchStep = 2;
        ticksPassed = 2;
        break;
    }
    case 2:
    {
        spriteFetchLow = memory->readMemory(spriteFetchAddress);
        oamFetchStep = 3;
        ticksPassed = 2;
        break;
    }
    case 3:
    {
        spriteFetchHigh = memory->readMemory(spriteFetchAddress + 1);
        oamFetchStep = 4;
        ticksPassed = 2;
        break;
    }
    case 4:
    {
        ticksPassed = 2;
        oamFetchStep = 5;
        break;
    }
    case 5:
    {
        if (pixelsPushed < 160 && oamFifo.size() == 0) {
            unsigned_two_byte spritePalette;
            if (/*currentSprite != nullptr && */(currentSprite.attributes >> 4) & 1) {
                spritePalette = memory->readMemory(0xFF49);
            }
            else {
                spritePalette = memory->readMemory(0xFF48);
            }
            oamFifo = decodeSpriteFetchData(spriteFetchHigh, spriteFetchLow, spritePalette);
            
            //Flip the sprite if necessary
            if ((currentSprite.attributes & 0x20) >> 5) { 
                std::stack<int> Stack;
                while (!oamFifo.empty()) {
                    Stack.push(oamFifo.front());
                    oamFifo.pop();
                }
                while (!Stack.empty()) {
                    oamFifo.push(Stack.top());
                    Stack.pop();
                }
            }
        }

        oamFetchStep = 1;
        isFetchingSprite = false;
        ticksPassed = 1;
        break;
    }
    }
    return ticksPassed;
}

void Ppu::oamScan()
{
    const unsigned_four_byte ioStart = 0xFF00;
    unsigned_two_byte ly = memory->readMemory(0xFF44);
    unsigned_two_byte spriteX = memory->readMemory(currentOamAddress + 1);
    unsigned_two_byte spriteY = memory->readMemory(currentOamAddress);
    if ((spriteX > 0) && (ly + 16 >= spriteY) && (ly + 16 < spriteY + getSpriteHeight()) && (oamBuffer.size() <= 10))
    {
        oamBuffer.push_back(Sprite(spriteY,
                                   spriteX,
                                   memory->readMemory(currentOamAddress + 2),
                                   memory->readMemory(currentOamAddress + 3),
                                   currentOamAddress));
    }
    currentOamAddress += 4;
}



int Ppu::getSpriteHeight()
{
    int spriteSize = ((memory->readMemory(0xFF00 + 0x40) & 0x4) >> 2);
    if (spriteSize == 1)
    {
        return 16;
    }
    else
    {
        return 8;
    }
}

std::queue<unsigned_two_byte> Ppu::decodeTileFetchData(unsigned_two_byte dataHigh, unsigned_two_byte dataLow, unsigned_two_byte colorPalette)
{
    std::queue<unsigned_two_byte> generatedTile;

    for (int i = 0; i < 8; i++)
    {
        unsigned_two_byte low = (dataHigh & (1 << (7 - i))) >> (7 - i);
        unsigned_two_byte high = (dataLow & (1 << (7 - i))) >> (7 - i);
        generatedTile.push((colorPalette >> 2 * ((high << 1) | low)) & 3);
    }
    return generatedTile;
}

std::queue<unsigned_two_byte> Ppu::decodeSpriteFetchData(unsigned_two_byte dataHigh, unsigned_two_byte dataLow, unsigned_two_byte colorPalette)
{
    std::queue<unsigned_two_byte> generatedTile;

    for (int i = 0; i < 8; i++)
    {
        unsigned_two_byte low = (dataLow & (1 << (7 - i))) >> (7 - i);
        unsigned_two_byte high = (dataHigh & (1 << (7 - i))) >> (7 - i);
        if (((high << 1) | low) == 0) {
            generatedTile.push(255);
        }
        else {
            generatedTile.push((colorPalette >> 2 * ((high << 1) | low)) & 3);
        }
    }
    return generatedTile;
}



std::vector<int> Ppu::decodeTile(std::vector<unsigned_two_byte> input)
{
    std::vector<int> tileA;
    std::vector<int> tileB;
    for (int i = 0; i < 16; i++)
    {
        if (i % 2 == 0)
        {
            tileA.push_back(input[i]);
        }
        else
        {
            tileB.push_back(input[i]);
        }
    }
    std::vector<int> generatedTile;

    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            int low = (tileA[y] & (1 << (7 - x))) >> (7 - x);
            int high = (tileB[y] & (1 << (7 - x))) >> (7 - x);
            generatedTile.push_back((high << 1) | low);
        }
    }
    return generatedTile;
}

/*-----------------------------------------------------------------------------------------------*/

std::vector<int> Ppu::getTiles()
{
    std::vector<int> output;
    for (int y = 0; y < 24; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            unsigned_four_byte base = 0x8000 + (x * 16) + (y * 256);
            std::vector<unsigned_two_byte> tileSet;
            for (int i = 0; i < 16; i++)
            {
                tileSet.push_back(memory->readMemory(base + i));
            }
            std::vector<int> decodedTile = decodeTile(tileSet);
            for (int i = 0; i < 64; i++)
            {
                output.push_back(decodedTile[i]);
            }
        }
    }
    return output;
}

std::vector<int> Ppu::getBackgroundTileNumbers()
{
    std::vector<int> output;

    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 32; x++)
        {
            int tileNumber;
            if (backgroundDebugAddress == 0x8000)
            {
                tileNumber = memory->readMemory(tilemapDebugAddress + (x)+(y * 32));
            }
            else
            {
                tileNumber = static_cast<signed_two_byte>(memory->readMemory(tilemapDebugAddress + (x)+(y * 32)));
                tileNumber += 256;
            }
            output.push_back(tileNumber);
        }
    }
    return output;
}

void Ppu::setDebugAddresses(unsigned_four_byte background, unsigned_four_byte tilemap)
{
    if (background != NULL)
    {
        backgroundDebugAddress = background;
    }
    
    if (tilemap != NULL)
    {
        tilemapDebugAddress = tilemap;
    }
    
}

std::vector<int> Ppu::getViewPort()
{
    return viewport;
}

void Ppu::clearViewPort()
{
    viewport.clear();
}

std::vector<Sprite> Ppu::getOam()
{
    std::vector<Sprite> output;
    const unsigned_four_byte oamStartAddress = 0xFE00;
    
    for (int i = 0; i < 40; i++)
    {
        int y = memory->readMemory(oamStartAddress + (i * 4));
        int x = memory->readMemory(oamStartAddress + (i * 4) + 1);
        int index = memory->readMemory(oamStartAddress + (i * 4) + 2);
        int attributes = memory->readMemory(oamStartAddress + (i * 4) + 3);
        Sprite nextItem(y, x, index, attributes, oamStartAddress + (i * 4));
        output.push_back(nextItem);
    }

    return output;
}

