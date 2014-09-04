/*    APU_triangle.h
    Author: Aaron Wiginton

    Description: APU Triangle for TinerasNES.
*/

#ifndef _APU_triangle_HEADER
#define _APU_triangle_HEADER

//using namespace std;

class APU_triangle
{
public:
    const static unsigned char lengthTable[];
    const static unsigned char triangleSequence[];

    // Samples Per Period Reset
    double samplesPerPeriod;
    int samplesCount;
    double seqPos;
    unsigned char volume;

    unsigned char linearCountTemp;
    bool linearHalt;

    // Triangle - 0x4008
    bool linearCounterDisable;    // Halt length counter
    unsigned char linearCount;    // Linear Count

    // Triangle - 0x4009
    /* Unused */

    // Triangle - 0x400A
    int period;                    // Period

    // Triangle - 0x400B
    unsigned char length;        // Length Index
    
    char renderSample();
    void updateLinearCounter(); 
    void updateFrequency();
    void updateLengthCounter();

    APU_triangle();
    ~APU_triangle();

    // Reset APU Variables
    void reset();

private:

    void init();    // Init APU
};
#endif //_APU_triangle_HEADER