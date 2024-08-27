#pragma once
#include "Types.hxx"
#include <vector>
#include <queue>
#include <list>
// #include "canvas.h"

class Memory;

struct Sprite
{
    Sprite() {};
    Sprite(int yPos, int xPos, int tileIndex, int attributes, int oamAddress)
    {
        this->yPos = yPos;
        this->xPos = xPos;
        this->tileIndex = tileIndex;
        this->attributes = attributes;
        this->oamAddress = oamAddress;
    };
    int yPos = 0;
    int xPos = 0;
    int tileIndex = 0;
    int attributes = 0;
    int oamAddress = 0;
};
class Ppu
{
private:
    Memory *memory = nullptr;
    // Determines if the next frame is ready to be drawn. Will be true after scanline 153 is complete.
    bool isFrameReady = false;
    int mode = 2;

    int currentScanline = 0;
    int currentScanlineTicks = 0;

    int currentOamAddress = 0;
    std::vector<Sprite> oamBuffer;
    std::vector<Sprite> currentSprites;
    Sprite currentSprite;
    bool isFetchingSprite = false;
    bool backgroundPriority = false;


    // unsigned_two_byte* viewport = new unsigned_two_byte[160*144];
    std::vector<int> viewport;
    int viewportXTile = 0; //x tile offset in relation to scx.
    int viewportYTile = 0; //y tile offset in relation to scy.
    int pixelsPushed = 0; //Number of pixels pushed to the viewport

    bool renderWindow = false;
    
    int backgroundFetchStep = 1;
    unsigned_four_byte tileFetchAddress;
    unsigned_two_byte tileFetchLow, tileFetchHigh;

    int oamFetchStep = 1;
    unsigned_four_byte spriteFetchAddress;
    unsigned_two_byte spriteFetchLow, spriteFetchHigh;

    std::queue<unsigned_two_byte> backgroundFifo;
    std::queue<unsigned_two_byte> oamFifo;

    int spriteXOffset = 0;

    unsigned_four_byte backgroundDebugAddress = 0x8000;
    unsigned_four_byte tilemapDebugAddress = 0x9800;

    int windowXOffset = 0, windowYOffset = 0;


public:
    Ppu(/* args */);
    ~Ppu();

    void setMemory(Memory *value);

    bool getIsFrameReady();
    void setIsFrameReady(bool isReady);

    // General function that drives the ppu. One pass of each mode lasts two clock cycles.
    void cycle();

    void modeZero();
    void modeOne();
    void modeTwo();
    void modeThree();

    int backgroundFetch();
    int spriteFetch();

    void oamScan();
    //bool spriteSorter(Sprite const& left, Sprite const& right);
    int getSpriteHeight();

    std::queue<unsigned_two_byte> decodeTileFetchData(unsigned_two_byte dataLow, unsigned_two_byte dataHigh, unsigned_two_byte colorPalette);
    std::queue<unsigned_two_byte> decodeSpriteFetchData(unsigned_two_byte dataLow, unsigned_two_byte dataHigh, unsigned_two_byte colorPalette);

    /*------------------------------------------------*/
    
    //returns a vector of tiles in sequential order as they apper in the vram memory
    std::vector<int> getTiles();
    // returns a vector of numbers that represent the 32x32 background map. top to bottom left to right. 
    std::vector<int> getBackgroundTileNumbers(); 

    //returns a vector of 8 2 bit numbers created from two bytes
    std::vector<int> decodeTile(std::vector<unsigned_two_byte> input);

    void setDebugAddresses(unsigned_four_byte background, unsigned_four_byte tilemap);

    std::vector<int> getViewPort();
    void clearViewPort();

    //returns a vector of the tile indexes of sprites in oam memory
    std::vector<Sprite> getOam();
};
