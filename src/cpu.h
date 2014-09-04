#include "common.h"

#ifndef _CPU_HEADER
#define _CPU_HEADER

//using namespace std;

class TinerasNES;
class MEM;

/* (P) 'P'rocessor Status Flags */
struct FlagBits
{
    bool c:1;        // Carry
    bool z:1;        // Zero
    bool i:1;        // Interrupt
    bool d:1;        // Decimal
    bool b:1;        // Break
    bool bit5:1;     // bit5
    bool v:1;        // Overflow
    bool n:1;        // Negative
};
union FlagsUnion
{
    FlagBits bits;
    unsigned char flags_byte;
};

class CPU
{
    /******  *****  REGISTERS ***** ******/
public:
    /* Declaration for Constructor    */
    CPU();
    ~CPU();

    // CPU Init Variables
    void init(MEM* mem);

    /* Declaration for CPU class method */
    // Get/Set (PC) Program Counter
    int getPC() { return _program_counter; }
    void setPC(int pc) { _program_counter = pc; }

    // Get/Set (pPC) PreviousProgram Counter
    int getPPC() { return _program_counter_previous; }
    void setPPC(int ppc) { _program_counter_previous = ppc; }

    // Get/Set (SP) Stack Pointer
    unsigned char getSP() { return _stack_pointer; }
    void setSP(int sp) { _stack_pointer = sp; }

    //// Get/Set (P) Processor Status (Flags)
    //unsigned char getProgramCounter() { return P; }
    //void setP(unsigned char p) { P = p; }

    // Get/Set (A) Accumulator
    unsigned char getA() { return _A; }
    void setA(unsigned char a) { _A = a; }

    // Get/Set (X) X Register
    unsigned char getX() { return _X; }
    void setX(unsigned char x) { _X = x; }

    // Get/Set (Y) Y Register
    unsigned char getY() { return _Y; }
    void setY(unsigned char y) { _Y = y; }

    int execOpCode();
    void writeNestest();

    // NMI & IRG
    int NMI();
    int IRQ();
    bool IRQPending;

    // Reset CPU Variables
    void reset(bool is_init=false);

    // Define FlagsUnion
    FlagsUnion flags;

private:
    // Check Flags
    bool checkCarry(int);    // Check to see if result had a Carry
    bool checkZero(int);    // Check to see if result was Zero
    bool checkOverflowADC(char, unsigned int, char);    // Check for Overflow (ADC)
    bool checkOverflowSBC(char, unsigned int, char);    // Check for Overflow (SBC)
    bool checkNegative(int);    // Check Sign
    bool checkNegative(unsigned int);    // Check Sign

    // Check Page Change
    void pageChangeCheck(int);    // Check for Page Change

    // Memory Addressing
    unsigned char addrAbs(unsigned char, unsigned char);  // Absolute Addressing Mode
    unsigned char addrAbsZP(unsigned char);  // Absolute[Zero-Page] Addressing Mode
    unsigned char addrIdx(unsigned char, unsigned char, unsigned char);  // Indexed Addressing Mode
    unsigned char addrIdxZP(unsigned char, unsigned char);  // Indexed[Zero-Page] Addressing Mode
    int           addrInd(unsigned char, unsigned char);  // Indirect Addressing Mode (ONLY USED BY JMP OPCODE)
    unsigned char addrIndX(unsigned char, unsigned char);  // Pre-Indexed Indirect Addressing Mode (Only X Register & Only ZERO-PAGE)
    unsigned char addrIndY(unsigned char, unsigned char);  // Post-Indexed Indirect Addressing Mode (Only Y Register & Only ZERO-PAGE)

    // Set Compare Flags
    void setCompareFlags(int, unsigned char);

    // Add High Byte, Low Byte and regXY and return int result
    int addHighLowRegXY(unsigned char, unsigned char, unsigned char);

    /* 6 Special and General Purpose Registers */
    int _program_counter;                // Program Counter
    int _program_counter_previous;            // Previous Program Counter Value
    unsigned char _stack_pointer;    // Stack Pointer
    //unsigned char P;    // Status Register ('P'rocessor Status)
    unsigned char _A;    // Accumulator
    unsigned char _X;    // Index Register X
    unsigned char _Y;    // Index Register Y

    int _previous_addrress;    // Hold previous address for Page Change checking

    // File stream for nestest file
    //ofstream nestestDebugStream;
    //int opCodeCounter;    // (FIX) remove me (8990 lines)

    // CPU Cycle Variables
    int _cpu_cycles;
    int _cpu_cycles_offset;
    int _page_changed_offset;

    // Primary OpCode and Two Operands
    unsigned char _byte_op_code;
    unsigned char _byte_one;
    unsigned char _byte_two;

    MEM* _mem;
};
#endif// _CPU_HEADER
