#include "cpu.h"
#include "tinerasnes.h"
#include "mem.h"

//    **********
//    OpCodes Execution Block
#pragma region /* ----- OpCodes Execution Block ----- */
int CPU::execOpCode()
{
    // N Z C I D V (sigN Zero Carry Interrupt Decim oVerflow)

    // Reset all cycle specific values
    _cpu_cycles = 0;
    _cpu_cycles_offset = 0;
    _page_changed_offset = 0;

    /* Copy Program Counter into Previous Program Counter
        Program counter stores the previous instruction.
        Not sure what's up with this yet.    */
    _program_counter_previous = _program_counter;
    _byte_op_code = _mem->getMEM(_program_counter_previous);
    _byte_one = _mem->getMEM(_program_counter_previous + 1);
    _byte_two = _mem->getMEM(_program_counter_previous + 2);

    #pragma region NESTEST and TRACE DEBUGGING
    // (DEBUG) Write nestest debug to file
    //int writeTo = 50000;
    //if(opCodeCounter++ <= writeTo)
    //{
    //    if(opCodeCounter != 1 && (PC != 0xFDA7) && (PC != 0xFDAA))
    //    {
    //        writeNestest();
    //    }
    //    else if((PC == 0xFDA7) || (PC == 0xFDAA))
    //    {
    //        opCodeCounter--;
    //    }
    //}
    //else
    //{
    //    tn->running = false;
    //    opCodeCounter = writeTo + 1;
    //}
    #pragma endregion

    #pragma region ALL OPCODES
    switch (_byte_op_code)
    {
        #pragma region OpCode ADC
        // ADC - Add memory to accumulator with Carry
        //  N Z C I D V
        //  / / / _ _ /
        case 0x69:  // ADC - Immediate
            {
                unsigned int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte
                
                // Add Accumulator + Byte Given + Carry
                tempA = (unsigned int)(_A + _byte_one + flags.bits.c);
                
                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowADC(_A, tempA, _byte_one);

                // Update Accumulator Value
                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x65:  // ADC - Absolute Zero-Page
            {
                unsigned int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Add Accumulator + Byte Given + Carry
                tempA = (unsigned int)(_A + addrAbsZP(_byte_one) + flags.bits.c);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowADC(_A, tempA, addrAbsZP(_byte_one));

                // Update Accumulator Value
                _A = (char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x75:  // ADC - Indexed Zero-Page (X)
            {
                unsigned int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Add Accumulator + Byte Given + Carry
                tempA = (unsigned int)(_A + addrIdxZP(_byte_one, _X) + flags.bits.c);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowADC(_A, tempA, addrIdxZP(_byte_one, _X));

                // Update Accumulator Value
                _A = (char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x6D:  // ADC - Absolute
            {
                unsigned int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Add Accumulator + Byte Given + Carry
                tempA = (unsigned int)(_A + addrAbs(_byte_one, _byte_two) + flags.bits.c);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowADC(_A, tempA, addrAbs(_byte_one, _byte_two));

                // Update Accumulator Value
                _A = (char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x7D:  // ADC - Absolute, Indexed (X)
            {
                unsigned int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Add Accumulator + Byte Given + Carry
                tempA = (unsigned int)(_A + addrIdx(_byte_one, _byte_two, _X) + flags.bits.c);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowADC(_A, tempA, addrIdx(_byte_one, _byte_two, _X));

                flags.bits.c = (tempA > 0xFF);

                // Update Accumulator Value
                _A = (char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x79:  // ADC - Absolute, Indexed (Y)
            {
                unsigned int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Add Accumulator + Byte Given + Carry
                tempA = (unsigned int)(_A + addrIdx(_byte_one, _byte_two, _Y) + flags.bits.c);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowADC(_A, tempA, addrIdx(_byte_one, _byte_two, _Y));

                // Update Accumulator Value
                _A = (char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x61:  // ADC - Indirect (X)
            {
                unsigned int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Add Accumulator + Byte Given + Carry
                tempA = (unsigned int)(_A + addrIndX(_byte_one, _X) + flags.bits.c);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowADC(_A, tempA, addrIndX(_byte_one, _X));

                // Update Accumulator Value
                _A = (char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x71:  // ADC - Indirect (Y)
            {
                unsigned int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Add Accumulator + Byte Given + Carry
                tempA = (unsigned int)(_A + addrIndY(_byte_one, _Y) + flags.bits.c);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowADC(_A, tempA, addrIndY(_byte_one, _Y));

                // Update Accumulator Value
                _A = (char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 5 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 2;
            }
            break;
        #pragma endregion  

        #pragma region OpCode AND
        // AND - AND Memory with Accumulator
        //  N Z C I D V
        //  / / _ _ _ _
        case 0x29:  // AND - Immediate
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // AND Accumulator + Byte Given
                tempA = _A & _byte_one;

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = (unsigned char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x25:  // AND - Absolute Zero-Page
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // AND Accumulator + Byte Given
                tempA = _A & addrAbsZP(_byte_one);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = (unsigned char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x35:  // AND - Indexed Zero-Page (X)
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // AND Accumulator + Byte Given
                tempA = _A & addrIdxZP(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = (unsigned char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x2D:  // AND - Absolute
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // AND Accumulator + Byte Given
                tempA = _A & addrAbs(_byte_one, _byte_two);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = (unsigned char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x3D:  // AND - Absolute, Indexed (X)
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // AND Accumulator + Byte Given
                tempA = _A & addrIdx(_byte_one, _byte_two, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = (unsigned char)(tempA & 0xFF);

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x39:  // AND - Absolute, Indexed (Y)
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // AND Accumulator + Byte Given
                tempA = _A & addrIdx(_byte_one, _byte_two, _Y);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = (unsigned char)(tempA & 0xFF);

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x21:  // AND - Indirect (X)
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // AND Accumulator + Byte Given
                tempA = _A & addrIndX(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = (unsigned char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x31:  // AND - Indirect (Y)
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // AND Accumulator + Byte Given
                tempA = _A & addrIndY(_byte_one, _Y);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = (unsigned char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 2;
            }
            break;
        #pragma endregion

        #pragma region OpCode ASL
        // ASL - ASL Shift Left One Bit (Accumulator or Memory)
        //  N Z C I D V
        //  / / / _ _ _
        case 0x0A:  // ASL - Accumulator
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Shift Memory or Accumulator One Bit to the Left
                tempA = _A * 2;

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = checkCarry(tempA);

                _A = (unsigned char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;

        case 0x06:  // ASL - Absolute Zero-Page
            {
                int intMem;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Shift Memory or Accumulator One Bit to the Left
                intMem = addrAbsZP(_byte_one) * 2;

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = checkCarry(intMem);

                _mem->setMEM(_byte_one, ((unsigned char)(intMem & 0xFF)));
                
                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x16:  // ASL - Indexed Zero-Page (X)
            {
                int intMem;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Shift Memory or Accumulator One Bit to the Left
                intMem = addrIdxZP(_byte_one, _X) * 2;

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = checkCarry(intMem);

                _mem->setMEM((unsigned char)(_byte_one + _X), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x0E:  // ASL - Absolute
            {
                int intMem;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Shift Memory or Accumulator One Bit to the Left
                intMem = addrAbs(_byte_one, _byte_two) * 2;

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = checkCarry(intMem);

                _mem->setMEM((_byte_one + ((_byte_two << 8))), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x1E:  // ASL - Absolute, Indexed (X)
            {
                int intMem;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Shift Memory or Accumulator One Bit to the Left
                intMem = addrIdx(_byte_one, _byte_two, _X) * 2;

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = checkCarry(intMem);

                _mem->setMEM((_byte_one + ((_byte_two << 8))) + _X, ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 7;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion

        #pragma region OpCode BCC
        // BCC - Branch on Carry Clear
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x90:  // BCC - Relative
            {
                // Add or Subtract from Program Counter if Carry Flag is CLEAR
                if (flags.bits.c == false)
                {
                    _program_counter += 2; 
                    _program_counter += (signed char)_byte_one;

                    //(FIX) SAMPLE CODE FOR sbyte/signed char crap
                      //pc += 2;
                      //if(zflag == 0)
                      //{
                         //  pc--;
                         //  if(memory[pc] > 0x7f) pc -= (~memory[pc] & 0x00ff);
                         //  else pc += (memory[pc] & 0x00ff);
                      //} 

                    pageChangeCheck(_program_counter);

                    _cpu_cycles_offset = 1 + _page_changed_offset;
                }                        
                else // If not clear, just update program counter
                {                        
                    // Update Program Counter
                    _program_counter += 2;
                }

                // Update CPU Cycles
                _cpu_cycles = 2 + _cpu_cycles_offset;
            }
            break;
        #pragma endregion

        #pragma region OpCode BCS
        // BCS - Branch on Carry Set
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0xB0:  // BCS - Relative
            {
                // Add or Subtract from Program Counter if Carry Flag is SET
                if (flags.bits.c == true)
                {
                    _program_counter += 2;
                    _program_counter += (signed char)_byte_one;

                    pageChangeCheck(_program_counter);

                    _cpu_cycles_offset = 1 + _page_changed_offset;
                }
                else // If not clear, just update program counter
                {
                    // Update Program Counter
                    _program_counter += 2;
                }

                // Update CPU Cycles
                _cpu_cycles = 2 + _cpu_cycles_offset;
            }
            break;
        #pragma endregion

        #pragma region OpCode BEQ
        // BEQ - Branch on Zero
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0xF0:  // BCS - Relative
            {
                // Add or Subtract from Program Counter if Zero Flag is SET
                if (flags.bits.z == true)
                {
                    _program_counter += 2;
                    _program_counter += (signed char)_byte_one;

                    pageChangeCheck(_program_counter);

                    _cpu_cycles_offset = 1 + _page_changed_offset;
                }
                else // If not clear, just update program counter
                {
                    // Update Program Counter
                    _program_counter += 2;
                }

                // Update CPU Cycles
                _cpu_cycles = 2 + _cpu_cycles_offset;
            }
            break;
        #pragma endregion

        #pragma region OpCode BIT
        // BIT - BIT Test Bits in Memory with Accumulator
        //  N Z C I D V
        //  M7/ _ _ _ M6
        case 0x24:  // BIT - Absolute Zero-Page
            {
                /* First AND Accu and Memory, then check to see if bits 6 and 7 equal 1
                   NOTE: When doing bitwise operations like the one below:
                   'A & addrAbsZP(byteOne)', you cannot directly place the
                   result into a BYTE in case of overflow. The compiler will not let you.
                   You must place it into a larger TYPE (int in this case) first and then
                   you can convert to a BYTE afterward*/
                char intTemp = /* A & */ addrAbsZP(_byte_one);

                // Set Zero Flag based on whether or not Bit 6 AND Bit 7 Equal 1
                if ((_A & intTemp) == 0)//(byteTemp & 0x80) == 0x80 && (byteTemp & 0x40) == 0x40)
                {
                    flags.bits.z = true;
                }
                else { flags.bits.z = false; }

                // Place Bit 7 value in Negative(sign) status bit
                if ((intTemp & 0x80) == 0x80)
                {
                    flags.bits.n = true;
                }
                else { flags.bits.n = false; }

                // Place Bit 6 value in Overflow status bit
                if ((intTemp & 0x40) == 0x40)
                {
                    flags.bits.v = true;
                }
                else { flags.bits.v = false; }

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x2C:  // BIT - Absolute
            {
                /* First AND Accu and Memory, then check to see if bits 6 and 7 equal 1
                   NOTE: When doing bitwise operations like the one below:
                   'A & addrAbsZP(byteOne)', you cannot directly place the
                   result into a BYTE in case of overflow. The compiler will not let you.
                   You must place it into a larger TYPE (int in this case) first and then
                   you can convert to a BYTE afterward*/
                char intTemp = /*A & */ addrAbs(_byte_one, _byte_two);

                // Set Zero Flag based on whether or not Bit 6 AND Bit 7 Equal 1
                if ((_A & intTemp) == 0)//(byteTemp & 0x80) == 0x80 && (byteTemp & 0x40) == 0x40)
                {
                    flags.bits.z = true;
                }
                else { flags.bits.z = false; }

                // Place Bit 7 value in Negative(sign) status bit
                if ((intTemp & 0x80) == 0x80)
                {
                    flags.bits.n = true;
                }
                else { flags.bits.n = false; }

                // Place Bit 6 value in Overflow status bit
                if ((intTemp & 0x40) == 0x40)
                {
                    flags.bits.v = true;
                }
                else { flags.bits.v = false; }

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion

        #pragma region OpCode BMI
        // BMI - Branch on Minus (Negative)
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x30:  // BCS - Relative
            {
                // Add or Subtract from Program Counter if Sign Flag is SET (Negative)
                if (flags.bits.n == true)
                {
                    _program_counter += 2;
                    _program_counter += (signed char)_byte_one;

                    pageChangeCheck(_program_counter);

                    _cpu_cycles_offset = 1 + _page_changed_offset;
                }
                else // If not clear, just update program counter
                {
                    // Update Program Counter
                    _program_counter += 2;
                }

                // Update CPU Cycles
                _cpu_cycles = 2 + _cpu_cycles_offset;
            }
            break;
        #pragma endregion

        #pragma region OpCode BNE
        // BNE - Branch on Result NOT Zero
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0xD0:  // BNE - Relative
            {
                // Add or Subtract from Program Counter if Zero Flag is NOT SET
                if (flags.bits.z == false)
                {
                    _program_counter += 2;
                    _program_counter += (signed char)_byte_one;

                    pageChangeCheck(_program_counter);

                    _cpu_cycles_offset = 1 + _page_changed_offset;
                }
                else // If not clear, just update program counter
                {
                    // Update Program Counter
                    _program_counter += 2;
                }

                // Update CPU Cycles
                _cpu_cycles = 2 + _cpu_cycles_offset;
            }
            break;
        #pragma endregion

        #pragma region OpCode BPL
        // BPL - Branch on Plus (Not Negative)
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x10:  // BPL - Relative
            {
                // Add or Subtract from Program Counter if Sign Flag is NOT SET (Positive)
                if (flags.bits.n == false)
                {
                    _program_counter += 2;
                    _program_counter += (signed char)_byte_one;

                    pageChangeCheck(_program_counter);

                    _cpu_cycles_offset = 1 + _page_changed_offset;
                }
                else // If not clear, just update program counter
                {
                    // Update Program Counter
                    _program_counter += 2;
                }

                // Update CPU Cycles
                _cpu_cycles = 2 + _cpu_cycles_offset;
            }
            break;
        #pragma endregion

        #pragma region OpCode BRK
        // BRK - Force Break
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x00:  // BRK - Implied
            {
                /* Several things happen when there is a Break
                    1. Program Counter increases by 1 to bypass the current instuction on return
                    2. The address in the Program Counter is PUSHed to the Stack
                    3. Break flag is set
                    4. Status Register is PUSHed to the Stack
                    5. Interrupt Disable Flag is SET to 1
                    6. Program Counter is Loaded with the default 'Break Address' 0xFFFE and 0xFFFF in this case
                    7. Interrupt is handled by the code at that address
                    8. Program Counter and Status Register are POPped from the Stack
                    9. Control is returned to our program and it continues where it left off
                 */

                // Increment Program Counter
                _program_counter++;
                _program_counter++;
                //flags.bits.b = false;
                // Push memory to stack
                _mem->stackPush(_stack_pointer, (unsigned char)(_program_counter >> 8)); // Push High Byte
                _mem->stackPush(_stack_pointer, (_program_counter & 0xFF));    // Push Low Byte

                // Set Break Flag (REMEMBER TO DO THIS **AFTER** pushing SR to the stack so interrupt is cleared upon return
                // ?????? ?????? ?????? ?????? ????? ????????????????????? Reference (pg 25) says to set it before pushing SR ?????
                // WIKI says BEFORE, so I put it before http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
                
                flags.bits.b = true;
                flags.bits.bit5 = true;
                // Push Status Register to Stack
                _mem->stackPush(_stack_pointer, flags.flags_byte);

                // Set Interrupt flag (Side effect after pushing according to wiki - http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior)
                flags.bits.i = true;

                // Set Program Counter to 0xFFFE and 0xFFFF to execute appropriate ISR
                /* This will get the Address of the next instructions to execute from
                   Memory location 0xFFFE and 0xFFFF.  It should automatically be instructed to
                   come back and continue where it left off after executing its ISR routine */
                _program_counter = _mem->getMEM(0xFFFE) | (_mem->getMEM(0xFFFF) << 8);

                // Update CPU Cycles
                _cpu_cycles = 7 + _cpu_cycles_offset;
            }
            break;
        #pragma endregion

        #pragma region OpCode BVC
        // BVC - Branch on Overflow Clear
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x50:  // BVC - Relative
            {
                // Add or Subtract from Program Counter if Overflow is Clear
                if (flags.bits.v == false)
                {
                    _program_counter += 2;
                    _program_counter += (signed char)_byte_one;

                    pageChangeCheck(_program_counter);

                    _cpu_cycles_offset = 1 + _page_changed_offset;
                }
                else // If not clear, just update program counter
                {
                    // Update Program Counter
                    _program_counter += 2;
                }

                // Update CPU Cycles
                _cpu_cycles = 2 + _cpu_cycles_offset;
            }
            break;
        #pragma endregion

        #pragma region OpCode BVS
        // BVS - Branch on Overflow Set
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x70:  // BVS - Relative
            {
                // Add or Subtract from Program Counter if Overflow is Set
                if (flags.bits.v == true)
                {
                    _program_counter += 2;
                    _program_counter += (signed char)_byte_one;

                    pageChangeCheck(_program_counter);

                    _cpu_cycles_offset = 1 + _page_changed_offset;
                }
                else // If not clear, just update program counter
                {
                    // Update Program Counter
                    _program_counter += 2;
                }

                // Update CPU Cycles
                _cpu_cycles = 2 + _cpu_cycles_offset;
            }
            break;
        #pragma endregion

        #pragma region OpCode CLC
        // CLC - Clear Carry Flag
        //  N Z C I D V
        //  _ _ 0 _ _ _
        case 0x18:  // CLC - Implied
            {
                // Clear Carry Flag
                flags.bits.c = false;

                // Update Program Counter
                _program_counter += 1;

                // Update CPU Cycles
                _cpu_cycles = 2;
            }
            break;
        #pragma endregion

        #pragma region OpCode CLD
        // CLD - Clear Decimal Flag
        //  N Z C I D V
        //  _ _ _ _ 0 _
        case 0xD8:  // CLD - Implied
            {
                // Clear Decimal Flag
                flags.bits.d = false;

                // Update Program Counter
                _program_counter += 1;

                // Update CPU Cycles
                _cpu_cycles = 2;
            }
            break;
        #pragma endregion

        #pragma region OpCode CLI
        // CLI - Clear Interrupt Disable Flag
        //  N Z C I D V
        //  _ _ _ 0 _ _
        case 0x58:  // CLI - Implied
            {
                // Clear Interrup Disable Flag
                flags.bits.i = false;

                // Update Program Counter
                _program_counter += 1;

                // Update CPU Cycles
                _cpu_cycles = 2;
            }
            break;
        #pragma endregion

        #pragma region OpCode CLV
        // CLV - Clear Overflow Flag
        //  N Z C I D V
        //  _ _ _ _ _ 0
        case 0xB8:  // CLV - Implied
            {
                // Clear Overflow Flag
                flags.bits.v = false;

                // Update Program Counter
                _program_counter += 1;

                // Update CPU Cycles
                _cpu_cycles = 2;
            }
            break;
        #pragma endregion

        #pragma region OpCode CMP
        // CMP - Compare Memory and Accumulator
        //  N Z C I D V
        //  / / / _ _ _
        case 0xC9:  // CMP - Immediate
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Accumulator (Subtract Memory from Accumulator)
                tempA = _A - _byte_one;

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                setCompareFlags(_A, _byte_one);

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xC5:  // CMP - Absolute Zero-Page
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Accumulator (Subtract Memory from Accumulator)
                tempA = _A - addrAbsZP(_byte_one);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                setCompareFlags(_A, addrAbsZP(_byte_one));

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xD5:  // CMP - Indexed Zero-Page (X)
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Accumulator (Subtract Memory from Accumulator)
                tempA = _A - addrIdxZP(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                setCompareFlags(_A, addrIdxZP(_byte_one, _X));

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xCD:  // CMP - Absolute
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Accumulator (Subtract Memory from Accumulator)
                tempA = _A - addrAbs(_byte_one, _byte_two);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                setCompareFlags(_A, addrAbs(_byte_one, _byte_two));

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xDD:  // CMP - Absolute, Indexed (X)
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Accumulator (Subtract Memory from Accumulator)
                tempA = _A - addrIdx(_byte_one, _byte_two, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                setCompareFlags(_A, addrIdx(_byte_one, _byte_two, _X));

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xD9:  // CMP - Absolute, Indexed (Y)
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Accumulator (Subtract Memory from Accumulator)
                tempA = _A - addrIdx(_byte_one, _byte_two, _Y);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                setCompareFlags(_A, addrIdx(_byte_one, _byte_two, _Y));

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xC1:  // CMP - Indirect (X)
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Accumulator (Subtract Memory from Accumulator)
                tempA = _A - addrIndX(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                setCompareFlags(_A, addrIndX(_byte_one, _X));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xD1:  // CMP - Indirect (Y)
            {
                int tempA;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Accumulator (Subtract Memory from Accumulator)
                tempA = _A - addrIndY(_byte_one, _Y);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                setCompareFlags(_A, addrIndY(_byte_one, _Y));

                // Update CPU Cycles
                
                _cpu_cycles = 5 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 2;
            }
            break;
        #pragma endregion 

        #pragma region OpCode CPX
        // CPX - Compare Memory and Register X
        //  N Z C I D V
        //  / / / _ _ _
        case 0xE0:  // CPX - Immediate
            {
                int intRegX;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Register X (Subtract Memory from Register X)
                intRegX = _X - _byte_one;

                // Set status bits
                flags.bits.n = checkNegative(intRegX & 0xFF);
                setCompareFlags(_X, _byte_one);

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xE4:  // CPX - Absolute Zero-Page
            {
                int intRegX;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Register X (Subtract Memory from Register X)
                intRegX = _X - addrAbsZP(_byte_one);

                // Set status bits
                flags.bits.n = checkNegative(intRegX & 0xFF);
                setCompareFlags(_X, addrAbsZP(_byte_one));

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xEC:  // CPX - Absolute
            {
                int intRegX;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Register X (Subtract Memory from Register X)
                intRegX = _X - addrAbs(_byte_one, _byte_two);

                // Set status bits
                flags.bits.n = checkNegative(intRegX & 0xFF);
                setCompareFlags(_X, addrAbs(_byte_one, _byte_two));

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion 

        #pragma region OpCode CPY
        // CPY - Compare Memory and Register Y
        //  N Z C I D V
        //  / / / _ _ _
        case 0xC0:  // CPY - Immediate
            {
                int intRegY;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Register X (Subtract Memory from Register Y)
                intRegY = _Y - _byte_one;

                // Set status bits
                flags.bits.n = checkNegative(intRegY & 0xFF);
                setCompareFlags(_Y, _byte_one);

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xC4:  // CPY - Absolute Zero-Page
            {
                int intRegY;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Register X (Subtract Memory from Register Y)
                intRegY = _Y - addrAbsZP(_byte_one);

                // Set status bits
                flags.bits.n = checkNegative(intRegY & 0xFF);
                setCompareFlags(_Y, addrAbsZP(_byte_one));

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xCC:  // CPY - Absolute
            {
                int intRegY;    // Temporary placeholder for Accum to check for N,Z,C,V before converting back to byte

                // Compare Memory and Register Y (Subtract Memory from Register Y)
                intRegY = _Y - addrAbs(_byte_one, _byte_two);

                // Set status bits
                flags.bits.n = checkNegative(intRegY & 0xFF);
                setCompareFlags(_Y, addrAbs(_byte_one, _byte_two));

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion 

        #pragma region OpCode DEC
        // DEC - Decrement Memory by 1
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xC6:  // DEC - Absolute Zero-Page
            {
                unsigned char byteMem;

                // Decrement 1 from Memory Contents of Memory Location byteOne
                byteMem = (unsigned char)(addrAbsZP(_byte_one) - 1);

                // Set status bits
                flags.bits.n = checkNegative(byteMem & 0xFF);
                flags.bits.z = checkZero(byteMem & 0xFF);

                // Write decremented value back to memory
                _mem->setMEM(_byte_one, (unsigned char)(byteMem & 0xFF));

                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xD6:  // DEC - Indexed Zero-Page (X)
            {
                unsigned char byteMem;

                // Decrement 1 from Memory Contents of Memory Location byteOne
                byteMem = (unsigned char)(addrIdxZP(_byte_one, _X) - 1);

                // Set status bits
                flags.bits.n = checkNegative(byteMem & 0xFF);
                flags.bits.z = checkZero(byteMem & 0xFF);

                // Write decremented value back to memory
                _mem->setMEM((unsigned char)(_byte_one + _X), (unsigned char)(byteMem & 0xFF));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xCE:  // DEC - Absolute
            {
                unsigned char byteMem;

                // Decrement 1 from Memory Contents of Memory Location byteOne
                byteMem = (unsigned char)(addrAbs(_byte_one, _byte_two) - 1);

                // Set status bits
                flags.bits.n = checkNegative(byteMem & 0xFF);
                flags.bits.z = checkZero(byteMem & 0xFF);

                // Write decremented value back to memory
                _mem->setMEM((_byte_one + ((_byte_two << 8))), (unsigned char)(byteMem & 0xFF));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xDE:  // DEC - Absolute, Indexed (X)
            {
                unsigned char byteMem;

                // Decrement 1 from Memory Contents of Memory Location byteOne
                byteMem = (unsigned char)(addrIdx(_byte_one, _byte_two, _X) - 1);

                // Set status bits
                flags.bits.n = checkNegative(byteMem & 0xFF);
                flags.bits.z = checkZero(byteMem & 0xFF);

                // Write decremented value back to memory
                _mem->setMEM((_byte_one + ((_byte_two << 8))) + _X, (unsigned char)(byteMem & 0xFF));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion

        #pragma region OpCode DEX
        // DEX - Decrement X by 1
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xCA:  // DEX - Immediate
            {
                //unsigned int intX = (unsigned int)(X - 1) & 0xFF;

                // Decrement 1 from X
                //X = (unsigned char)(intX);
                _X = (unsigned char)(_X - 1);

                // Set status bits
                flags.bits.n = checkNegative(_X & 0xFF);
                flags.bits.z = checkZero(_X & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode DEY
        // DEY - Decrement Y by 1
        //  N Z C I D V
        //  / / _ _ _ _
        case 0x88:  // DEY - Immediate
            {
                //unsigned int intY = (unsigned int)(Y - 1) & 0xFF;

                // Decrement 1 from Y
                //Y = (unsigned char)(intY);
                _Y = (unsigned char)(_Y - 1);

                // Set status bits
                flags.bits.n = checkNegative(_Y & 0xFF);
                flags.bits.z = checkZero(_Y & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode EOR
        // EOR - XOR memory with accumulator
        //  N Z C I D V
        //  / / / _ _ /
        case 0x49:  // EOR - Immediate
            {
                int tempA;

                // XOR Memory with Accumulator
                tempA = _A ^ _byte_one;

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x45:  // EOR - Absolute Zero-Page
            {
                int tempA;

                // XOR Memory with Accumulator
                tempA = _A ^ addrAbsZP(_byte_one);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x55:  // EOR - Indexed Zero-Page (X)
            {
                int tempA;

                // XOR Memory with Accumulator
                tempA = _A ^ addrIdxZP(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x4D:  // EOR - Absolute
            {
                int tempA;

                // XOR Memory with Accumulator
                tempA = _A ^ addrAbs(_byte_one, _byte_two);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x5D:  // EOR - Absolute, Indexed (X)
            {
                int tempA;

                // XOR Memory with Accumulator
                tempA = _A ^ addrIdx(_byte_one, _byte_two, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x59:  // EOR - Absolute, Indexed (Y)
            {
                int tempA;

                // XOR Memory with Accumulator
                tempA = _A ^ addrIdx(_byte_one, _byte_two, _Y);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x41:  // EOR - Indirect (X)
            {
                int tempA;

                // XOR Memory with Accumulator
                tempA = _A ^ addrIndX(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x51:  // EOR - Indirect (Y)
            {
                int tempA;

                // XOR Memory with Accumulator
                tempA = _A ^ addrIndY(_byte_one, _Y);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 5 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 2;
            }
            break;
        #pragma endregion  

        #pragma region OpCode INC
        // INC - Increment Memory by 1
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xE6:  // INC - Absolute Zero-Page
            {
                char byteMem;

                // Increment Memory Contents of Memory Location byteOne by 1
                byteMem = (unsigned char)(addrAbsZP(_byte_one) + 1);

                // Set status bits
                flags.bits.n = checkNegative(byteMem & 0xFF);
                flags.bits.z = checkZero(byteMem & 0xFF);

                // Write incremented value back to memory
                _mem->setMEM(_byte_one, (unsigned char)(byteMem & 0xFF));

                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xF6:  // INC - Indexed Zero-Page (X)
            {
                char byteMem;

                // Increment Memory Contents of Memory Location byteOne by 1
                byteMem = (unsigned char)(addrIdxZP(_byte_one, _X) + 1);

                // Set status bits
                flags.bits.n = checkNegative(byteMem & 0xFF);
                flags.bits.z = checkZero(byteMem & 0xFF);

                // Write incremented value back to memory
                _mem->setMEM((unsigned char)(_byte_one + _X), (unsigned char)(byteMem & 0xFF));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xEE:  // INC - Absolute
            {
                char byteMem;

                // Increment Memory Contents of Memory Location byteOne by 1
                byteMem = (unsigned char)(addrAbs(_byte_one, _byte_two) + 1);

                // Set status bits
                flags.bits.n = checkNegative(byteMem & 0xFF);
                flags.bits.z = checkZero(byteMem & 0xFF);

                // Write incremented value back to memory
                _mem->setMEM((_byte_one + (_byte_two << 8)), (unsigned char)(byteMem & 0xFF));

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xFE:  // INC - Absolute, Indexed (X)
            {
                char byteMem;

                // Decrement 1 from Memory Contents of Memory Location byteOne
                byteMem = (unsigned char)(addrIdx(_byte_one, _byte_two, _X) + 1);

                // Set status bits
                flags.bits.n = checkNegative(byteMem & 0xFF);
                flags.bits.z = checkZero(byteMem & 0xFF);

                // Write decremented value back to memory
                _mem->setMEM((_byte_one + (_byte_two << 8)) + _X, (unsigned char)(byteMem & 0xFF));

                // Update CPU Cycles
                _cpu_cycles = 7;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion

        #pragma region OpCode INX
        // INX - Increment X by 1
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xE8:  // INX - Immediate
            {
                //unsigned int intX = (unsigned int)(X + 1) & 0xFF;

                // Increment X by 1
                //X = (unsigned char)(intX);
                _X = (unsigned char)(_X + 1);

                // Set status bits
                flags.bits.n = checkNegative(_X & 0xFF);
                flags.bits.z = checkZero(_X & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode INY
        // INY - Increment Y by 1
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xC8:  // INY - Immediate
            {
                //unsigned int intY = (unsigned int)(Y + 1) & 0xFF;

                // Increment Y by 1
                //Y = (char)(intY);
                _Y = (unsigned char)(_Y + 1);

                // Set status bits
                flags.bits.n = checkNegative(_Y & 0xFF);
                flags.bits.z = checkZero(_Y & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode JMP
        // JMP - Jump to new location
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x4C:  // JMP - Absolute
            {
                int intMem = 0x0000;    // Temporary Storage Location for Memory Address to Jump To

                if (_byte_one == 0xb8)
                {
                    _byte_one = _byte_one;
                }

                // Get Low and High Byte of new memory location
                intMem = _byte_one + ((_byte_two << 8));  // Add High Byte to Low Byte

                _program_counter = intMem;    // Set Program Counter to new location

                // Update CPU Cycles
                _cpu_cycles = 3;
            }
            break;

        case 0x6C:  // JMP - Indirect
            {
                int intMem = 0x0000;    // Temporary Storage Location for Memory Address to Jump To

                // Get Low and High Byte of new memory location
                intMem = addrAbs(_byte_one, _byte_two);             // Low Byte
                intMem += (addrAbs(++_byte_one, _byte_two) << 8);  // Add High Byte to Low Byte

                _program_counter = intMem;    // Set Program Counter to new location

                // Update CPU Cycles
                _cpu_cycles = 3;
            }
            break;
        #pragma endregion

        #pragma region OpCode JSR
        // JSR - Jump to new location and Save Current Address
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x20:  // JSR - Absolute
            {
                int intMem = 0x0000;    // Temporary Storage Location for Memory Address to Jump To

                // Decrement Program Counter
                _program_counter += 2;
                // Says pc-- in ref material because the program counter automatically updates when opcodes are executed.
                // This means that instead of pointing to the next instruction PC+3, we tell it to decrement(pc--) or in our case only add 2.
                // We do this because when the RTS instruction is executed, it automatically increments the PC value that is
                // stored on the stack.  So when we return, the PC is now at PC+3 (or the next instruction).

                // Push memory to stack
                _mem->stackPush(_stack_pointer, (unsigned char)(_program_counter >> 8)); // Push High Byte
                _mem->stackPush(_stack_pointer, (unsigned char)(_program_counter & 0xFF));    // Push Low Byte

                // Get Low and High Byte of new memory location
                intMem = _byte_one + (_byte_two << 8);  // Add High Byte to Low Byte

                _program_counter = intMem;    // Set Program Counter to new location

                // Update CPU Cycles
                _cpu_cycles = 6;
            }
            break;
        #pragma endregion

        #pragma region OpCode LDA
        // LDA - Load Memory into Accumulator
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xA9:  // LDA - Immediate
            {
                int tempA;

                // Load Memory into Accumulator
                tempA = _byte_one;

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xA5:  // LDA - Absolute Zero-Page
            {
                int tempA;

                // Load Memory into Accumulator
                tempA = addrAbsZP(_byte_one);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xB5:  // LDA - Indexed Zero-Page (X)
            {
                int tempA;

                // Load Memory into Accumulator
                tempA = addrIdxZP(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xAD:  // LDA - Absolute
            {
                int tempA;

                // Load Memory into Accumulator
                tempA = addrAbs(_byte_one, _byte_two);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xBD:  // LDA - Absolute, Indexed (X)
            {
                int tempA;

                // Load Memory into Accumulator
                tempA = addrIdx(_byte_one, _byte_two, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xB9:  // LDA - Absolute, Indexed (Y)
            {
                int tempA;

                // Load Memory into Accumulator
                tempA = addrIdx(_byte_one, _byte_two, _Y);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xA1:  // LDA - Indirect (X)
            {
                int tempA;

                // Load Memory into Accumulator
                tempA = addrIndX(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xB1:  // LDA - Indirect (Y)
            {
                int tempA;

                // Load Memory into Accumulator
                tempA = addrIndY(_byte_one, _Y);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 5 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 2;
            }
            break;
        #pragma endregion  
            
        #pragma region OpCode LDX
        // LDX - Load Memory into X
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xA2:  // LDX - Immediate
            {
                int intRegX;

                // Load Memory into X
                intRegX = _byte_one;

                // Set status bits
                flags.bits.n = checkNegative(intRegX & 0xFF);
                flags.bits.z = checkZero(intRegX & 0xFF);

                _X = intRegX & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xA6:  // LDX - Absolute Zero-Page
            {
                int intRegX;

                // Load Memory into X
                intRegX = addrAbsZP(_byte_one);

                // Set status bits
                flags.bits.n = checkNegative(intRegX & 0xFF);
                flags.bits.z = checkZero(intRegX & 0xFF);

                _X = intRegX & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xB6:  // LDX - Indexed Zero-Page (Y)
            {
                int intRegX;

                // Load Memory into X
                intRegX = addrIdxZP(_byte_one, _Y);

                // Set status bits
                flags.bits.n = checkNegative(intRegX & 0xFF);
                flags.bits.z = checkZero(intRegX & 0xFF);

                _X = intRegX & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xAE:  // LDX - Absolute
            {
                int intRegX;

                // Load Memory into X
                intRegX = addrAbs(_byte_one, _byte_two);

                // Set status bits
                flags.bits.n = checkNegative(intRegX & 0xFF);
                flags.bits.z = checkZero(intRegX & 0xFF);

                _X = intRegX & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xBE:  // LDX - Absolute, Indexed (Y)
            {
                int intRegX;

                // Load Memory into X
                intRegX = addrIdx(_byte_one, _byte_two, _Y);

                // Set status bits
                flags.bits.n = checkNegative(intRegX & 0xFF);
                flags.bits.z = checkZero(intRegX & 0xFF);

                _X = intRegX & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion  
                  
        #pragma region OpCode LDY
        // LDY - Load Memory into Y
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xA0:  // LDY - Immediate
            {
                int intRegY;

                // Load Memory into Y
                intRegY = _byte_one;

                // Set status bits
                flags.bits.n = checkNegative(intRegY & 0xFF);
                flags.bits.z = checkZero(intRegY & 0xFF);

                _Y = intRegY & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xA4:  // LDY - Absolute Zero-Page
            {
                int intRegY;

                // Load Memory into Y
                intRegY = addrAbsZP(_byte_one);

                // Set status bits
                flags.bits.n = checkNegative(intRegY & 0xFF);
                flags.bits.z = checkZero(intRegY & 0xFF);

                _Y = intRegY & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xB4:  // LDY - Indexed Zero-Page (X)
            {
                int intRegY;

                // Load Memory into Y
                intRegY = addrIdxZP(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(intRegY & 0xFF);
                flags.bits.z = checkZero(intRegY & 0xFF);

                _Y = intRegY & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xAC:  // LDY - Absolute
            {
                int intRegY;

                // Load Memory into Y
                intRegY = addrAbs(_byte_one, _byte_two);

                // Set status bits
                flags.bits.n = checkNegative(intRegY & 0xFF);
                flags.bits.z = checkZero(intRegY & 0xFF);

                _Y = intRegY & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xBC:  // LDY - Absolute, Indexed (X)
            {
                int intRegY;

                // Load Memory into Y
                intRegY = addrIdx(_byte_one, _byte_two, _X);

                // Set status bits
                flags.bits.n = checkNegative(intRegY & 0xFF);
                flags.bits.z = checkZero(intRegY & 0xFF);

                _Y = intRegY & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion 
  
        #pragma region OpCode LSR
        // LSR - LSR Shift Right One Bit (Accumulator or Memory)
        //  N Z C I D V
        //  / / / _ _ _
        case 0x4A:  // LSR - Accumulator
            {
                int tempA;

                // Check Low Bit for Carry to the Right before doing Shift Right
                if ((_A & 0x01) == 0x01)
                {
                    flags.bits.c =  true;
                }
                else { flags.bits.c = false; }

                // Shift Memory or Accumulator One Bit to the Right
                tempA = _A / 2;

                // Set status bits
                flags.bits.n = false;
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;

        case 0x46:  // LSR - Absolute Zero-Page
            {
                int intMem;

                // Get Memory before Shift Right to Check Carry Flag
                intMem = addrAbsZP(_byte_one);

                // Check Low Bit for Carry to the Right before doing Shift Right
                if ((intMem & 0x01) == 0x01)
                {
                    flags.bits.c = true;
                }
                else { flags.bits.c = false; }

                // Shift Memory or Accumulator One Bit to the Right
                intMem = intMem / 2;

                // Set status bits
                flags.bits.n = false;
                flags.bits.z = checkZero(intMem & 0xFF);

                _mem->setMEM((int)(_byte_one), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x56:  // LSR - Indexed Zero-Page (X)
            {
                int intMem;

                // Get Memory before Shift Right to Check Carry Flag
                intMem = addrIdxZP(_byte_one, _X);

                // Check Low Bit for Carry to the Right before doing Shift Right
                if ((intMem & 0x01) == 0x01)
                {
                    flags.bits.c = true;
                }
                else { flags.bits.c = false; }

                // Shift Memory or Accumulator One Bit to the Right
                intMem = intMem / 2;

                // Set status bits
                flags.bits.n = false;
                flags.bits.z = checkZero(intMem & 0xFF);

                _mem->setMEM((unsigned char)(_byte_one + _X), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x4E:  // LSR - Absolute
            {
                int intMem;

                // Get Memory before Shift Right to Check Carry Flag
                intMem = addrAbs(_byte_one, _byte_two);

                // Check Low Bit for Carry to the Right before doing Shift Right
                if ((intMem & 0x01) == 0x01)
                {
                    flags.bits.c = true;
                }
                else { flags.bits.c = false; }

                // Shift Memory or Accumulator One Bit to the Right
                intMem = intMem / 2;

                // Set status bits
                flags.bits.n = false;
                flags.bits.z = checkZero(intMem & 0xFF);

                _mem->setMEM((_byte_one + (_byte_two << 8)), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x5E:  // LSR - Absolute, Indexed (X)
            {
                int intMem;

                // Get Memory before Shift Right to Check Carry Flag
                intMem = addrIdx(_byte_one, _byte_two, _X);

                // Check Low Bit for Carry to the Right before doing Shift Right
                if ((intMem & 0x01) == 0x01)
                {
                    flags.bits.c = true;
                }
                else { flags.bits.c = false; }

                // Shift Memory or Accumulator One Bit to the Right
                intMem = intMem / 2;

                // Set status bits
                flags.bits.n = false;
                flags.bits.z = checkZero(intMem & 0xFF);

                _mem->setMEM((_byte_one + (_byte_two << 8)) + _X, ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 7;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion

        #pragma region OpCode NOP
        // NOP - No OPeration (2 CPU Cycles)
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0xEA:  // NOP - Implied
        case 0x1A:
        case 0x3A:
        case 0x5A:
        case 0x7A:
        case 0xDA:
        case 0xFA:
            {
                // Do Nothing for 2 CPU Cycles - ADD CODE HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;

        case 0x04:  // NOP
        case 0x14:
        case 0x34:
        case 0x44:
        case 0x54:
        case 0x64:
        case 0x74:
        case 0xD4:
        case 0xF4:
        {
            // Do Nothing for 3 CPU Cycles - ADD CODE HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            // Update CPU Cycles
            _cpu_cycles = 3;

            // Update Program Counter
            _program_counter += 2;
        }
        break;

        case 0x0C:  // NOP
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
        {
            // Do Nothing for 4 CPU Cycles - ADD CODE HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            // Update CPU Cycles
            _cpu_cycles = 4;

            // Update Program Counter
            _program_counter += 3;
        }
        break;
        #pragma endregion

        #pragma region OpCode ORA
        // ORA - OR Memory with Accumulator
        //  N Z C I D V
        //  / / _ _ _ _
        case 0x09:  // ORA - Immediate
            {
                int tempA;

                // OR Accumulator + Byte Given
                tempA = _A | _byte_one;

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x05:  // ORA - Absolute Zero-Page
            {
                int tempA;

                // OR Accumulator + Byte Given
                tempA = _A | addrAbsZP(_byte_one);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x15:  // ORA - Indexed Zero-Page (X)
            {
                int tempA;

                // OR Accumulator + Byte Given
                tempA = _A | addrIdxZP(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x0D:  // ORA - Absolute
            {
                int tempA;

                // OR Accumulator + Byte Given
                tempA = _A | addrAbs(_byte_one, _byte_two);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x1D:  // ORA - Absolute, Indexed (X)
            {
                int tempA;

                // OR Accumulator + Byte Given
                tempA = _A | addrIdx(_byte_one, _byte_two, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x19:  // ORA - Absolute, Indexed (Y)
            {
                int tempA;

                // OR Accumulator + Byte Given
                tempA = _A | addrIdx(_byte_one, _byte_two, _Y);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x01:  // ORA - Indirect (X)
            {
                int tempA;

                // OR Accumulator + Byte Given
                tempA = _A | addrIndX(_byte_one, _X);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x11:  // ORA - Indirect (Y)
            {
                int tempA;

                // OR Accumulator + Byte Given
                tempA = _A | addrIndY(_byte_one, _Y);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 2;
            }
            break;
        #pragma endregion

        #pragma region OpCode PHA
        // PHA - Push Accumulator to Stack
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x48:  // PHA - Implied
            {
                // Push Accumulator to Stack
                _mem->stackPush(_stack_pointer, _A);

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode PHP
        // PHP - Push Status Register to Stack
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x08:  // PHP - Implied
            {
                // Set BREAK and BIT5 before pushing to stack (according to wiki - http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior)
                flags.bits.b = true;
                //flags.bits.bit5 = true;

                // Set Interrupt flag (Side effect after pushing according to wiki - http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior)
                //flags.bits.i = true;

                // Push Status Register to Stack
                _mem->stackPush(_stack_pointer, flags.flags_byte);

                // Clear BREAK after pushing to stack
                flags.bits.b = false;

                // Update Status Register Byte Before Pushing to Stack
                //registers.setStatusRegister();

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode PLA
        // PLA - Pull Accumulator from Stack
        //  N Z C I D V
        //  / / _ _ _ _
        case 0x68:  // PLA - Implied
            {
                // Pull Accumulator from Stack
                _A = _mem->stackPop(_stack_pointer);

                // Set status bits
                flags.bits.n = checkNegative(_A);
                flags.bits.z = checkZero(_A);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode PLP
        // PLP - Pull Status Register from Stack
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x28:  // PLP - Implied
            {
                // Store bits 4 and 5 because they are ignored on PLP stack PoP (http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior)
                bool tempBreak = flags.bits.b;
                bool tempBitFive = flags.bits.bit5;

                // Pull Accumulator from Stack
                flags.flags_byte = _mem->stackPop(_stack_pointer);

                // Restore bit 4 and 5
                flags.bits.b = tempBreak;
                flags.bits.bit5 = tempBitFive;

                //// Set status bits
                //flags.bits.n = checkNegative(A);
                //flags.bits.z = checkZero(A);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode ROL
        // ROL - ROL Rotate Left One Bit (Accumulator or Memory) with Carry
        //  N Z C I D V
        //  / / / _ _ _
        case 0x2A:  // ROL - Accumulator
            {
                int tempA;

                /* The Carry bit (bit 7) is read AND changed in this operation, but cannot
                   be done in the same step.  We must read the initial Accum/Memory
                   value in first to check whether the Carry bit will be filled by
                   the Rotate. Then we Rotate (including the current Carry) and then
                   set the Carry bit after the operation is complete. */
                bool bolRegACarry;

                // Get Memory or Accumulator to Check for Potential Carry
                tempA = _A;

                // Set Carry based on tempA
                if ((tempA & 0x80) == 0x80)
                {
                    bolRegACarry = true;
                }
                else { bolRegACarry = false; }
                
                // Shift Memory or Accumulator One Bit to the Left
                tempA = tempA * 2 + flags.bits.c;

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = bolRegACarry;

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;

        case 0x26:  // ROL - Absolute Zero-Page
            {
                int intMem;

                /* The Carry bit (bit 7) is read AND changed in this operation, but cannot
                be done in the same step.  We must read the initial Accum/Memory
                value in first to check whether the Carry bit will be filled by
                the Rotate. Then we Rotate (including the current Carry) and then
                set the Carry bit after the operation is complete. */
                bool bolRegACarry;

                // Get Memory or Accumulator to Check for Potential Carry
                intMem = addrAbsZP(_byte_one);

                // Set Carry based on tempA
                if ((intMem & 0x80) == 0x80)
                {
                    bolRegACarry = true;
                }
                else { bolRegACarry = false; }

                // Shift Memory or Accumulator One Bit to the Left
                intMem = addrAbsZP(_byte_one) * 2 + flags.bits.c;

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = bolRegACarry;

                _mem->setMEM((int)(_byte_one), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x36:  // ROL - Indexed Zero-Page (X)
            {
                int intMem;

                /* The Carry bit (bit 7) is read AND changed in this operation, but cannot
                be done in the same step.  We must read the initial Accum/Memory
                value in first to check whether the Carry bit will be filled by
                the Rotate. Then we Rotate (including the current Carry) and then
                set the Carry bit after the operation is complete. */
                bool bolRegACarry;

                // Get Memory or Accumulator to Check for Potential Carry
                intMem = addrIdxZP(_byte_one, _X);

                // Set Carry based on tempA
                if ((intMem & 0x80) == 0x80)
                {
                    bolRegACarry = true;
                }
                else { bolRegACarry = false; }

                // Shift Memory or Accumulator One Bit to the Left
                intMem = addrIdxZP(_byte_one, _X) * 2 + flags.bits.c;

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = bolRegACarry;

                _mem->setMEM((unsigned char)(_byte_one + _X), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x2E:  // ROL - Absolute
            {
                int intMem;

                /* The Carry bit (bit 7) is read AND changed in this operation, but cannot
                be done in the same step.  We must read the initial Accum/Memory
                value in first to check whether the Carry bit will be filled by
                the Rotate. Then we Rotate (including the current Carry) and then
                set the Carry bit after the operation is complete. */
                bool bolRegACarry;

                // Get Memory or Accumulator to Check for Potential Carry
                intMem = addrAbs(_byte_one, _byte_two);

                // Set Carry based on tempA
                if ((intMem & 0x80) == 0x80)
                {
                    bolRegACarry = true;
                }
                else { bolRegACarry = false; }

                // Shift Memory or Accumulator One Bit to the Left
                intMem = addrAbs(_byte_one, _byte_two) * 2 + flags.bits.c;

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = bolRegACarry;

                _mem->setMEM((_byte_one + (_byte_two << 8)), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x3E:  // ROL - Absolute, Indexed (X)
            {
                int intMem;

                /* The Carry bit (bit 7) is read AND changed in this operation, but cannot
                be done in the same step.  We must read the initial Accum/Memory
                value in first to check whether the Carry bit will be filled by
                the Rotate. Then we Rotate (including the current Carry) and then
                set the Carry bit after the operation is complete. */
                bool bolRegACarry;

                // Get Memory or Accumulator to Check for Potential Carry
                intMem = addrIdx(_byte_one, _byte_two, _X);

                // Set Carry based on tempA
                if ((intMem & 0x80) == 0x80)
                {
                    bolRegACarry = true;
                }
                else { bolRegACarry = false; }

                // Shift Memory or Accumulator One Bit to the Left
                intMem = addrIdx(_byte_one, _byte_two, _X) * 2 + flags.bits.c;

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = bolRegACarry;

                _mem->setMEM((_byte_one + (_byte_two << 8)) + _X, ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 7;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion

        #pragma region OpCode ROR
        // ROR - ROR Rotate Right One Bit (Accumulator or Memory) with Carry
        //  N Z C I D V
        //  / / / _ _ _
        case 0x6A:  // ROR - Accumulator
            {
                int tempA;

                /* The Carry bit (bit 0) is read AND changed in this operation, but cannot
                   be done in the same step.  We must read the initial Accum/Memory
                   value in first to check whether the Carry bit will be filled by
                   the Rotate. Then we Rotate (including the current Carry) and then
                   set the Carry bit after the operation is complete. */
                bool bolRegACarry;

                // Get Memory or Accumulator to Check for Potential Carry
                tempA = _A;

                // Set Carry based on tempA
                if ((tempA & 0x01) == 0x01)
                {
                    bolRegACarry = true;
                }
                else { bolRegACarry = false; }

                // Shift Memory or Accumulator One Bit to the Left
                // Multiply Carry by 128 to place it into Bit 7 place before adding.
                tempA = tempA / 2 + (flags.bits.c * 128);

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                flags.bits.c = bolRegACarry;

                _A = (char)(tempA & 0xFF);

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;

        case 0x66:  // ROR - Absolute Zero-Page
            {
                int intMem;

                /* The Carry bit (bit 0) is read AND changed in this operation, but cannot
                be done in the same step.  We must read the initial Accum/Memory
                value in first to check whether the Carry bit will be filled by
                the Rotate. Then we Rotate (including the current Carry) and then
                set the Carry bit after the operation is complete. */
                bool bolRegACarry;

                // Get Memory or Accumulator to Check for Potential Carry
                intMem = addrAbsZP(_byte_one);

                // Set Carry based on tempA
                if ((intMem & 0x01) == 0x01)
                {
                    bolRegACarry = true;
                }
                else { bolRegACarry = false; }

                // Shift Memory or Accumulator One Bit to the Left
                // Multiply Carry by 128 to place it into Bit 7 place before adding.
                intMem = addrAbsZP(_byte_one) / 2 + (flags.bits.c * 128);

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = bolRegACarry;

                _mem->setMEM((int)(_byte_one), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x76:  // ROR - Indexed Zero-Page (X)
            {
                int intMem;

                /* The Carry bit (bit 0) is read AND changed in this operation, but cannot
                be done in the same step.  We must read the initial Accum/Memory
                value in first to check whether the Carry bit will be filled by
                the Rotate. Then we Rotate (including the current Carry) and then
                set the Carry bit after the operation is complete. */
                bool bolRegACarry;

                // Get Memory or Accumulator to Check for Potential Carry
                intMem = addrIdxZP(_byte_one, _X);

                // Set Carry based on tempA
                if ((intMem & 0x01) == 0x01)
                {
                    bolRegACarry = true;
                }
                else { bolRegACarry = false; }

                // Shift Memory or Accumulator One Bit to the Left
                // Multiply Carry by 128 to place it into Bit 7 place before adding.
                intMem = addrIdxZP(_byte_one, _X) / 2 + (flags.bits.c * 128);

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = bolRegACarry;

                _mem->setMEM((unsigned char)(_byte_one + _X), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x6E:  // ROR - Absolute
            {
                int intMem;

                /* The Carry bit (bit 0) is read AND changed in this operation, but cannot
                be done in the same step.  We must read the initial Accum/Memory
                value in first to check whether the Carry bit will be filled by
                the Rotate. Then we Rotate (including the current Carry) and then
                set the Carry bit after the operation is complete. */
                bool bolRegACarry;

                // Get Memory or Accumulator to Check for Potential Carry
                intMem = addrAbs(_byte_one, _byte_two);

                // Set Carry based on tempA
                if ((intMem & 0x01) == 0x01)
                {
                    bolRegACarry = true;
                }
                else { bolRegACarry = false; }

                // Shift Memory or Accumulator One Bit to the Left
                // Multiply Carry by 128 to place it into Bit 7 place before adding.
                intMem = addrAbs(_byte_one, _byte_two) / 2 + (flags.bits.c * 128);

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = bolRegACarry;

                _mem->setMEM((_byte_one + (_byte_two << 8)), ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x7E:  // ROR - Absolute, Indexed (X)
            {
                int intMem;

                /* The Carry bit (bit 0) is read AND changed in this operation, but cannot
                be done in the same step.  We must read the initial Accum/Memory
                value in first to check whether the Carry bit will be filled by
                the Rotate. Then we Rotate (including the current Carry) and then
                set the Carry bit after the operation is complete. */
                bool bolRegACarry;

                // Get Memory or Accumulator to Check for Potential Carry
                intMem = addrIdx(_byte_one, _byte_two, _X);

                // Set Carry based on tempA
                if ((intMem & 0x01) == 0x01)
                {
                    bolRegACarry = true;
                }
                else { bolRegACarry = false; }

                // Shift Memory or Accumulator One Bit to the Left
                // Multiply Carry by 128 to place it into Bit 7 place before adding.
                intMem = addrIdx(_byte_one, _byte_two, _X) / 2 + (flags.bits.c * 128);

                // Set status bits
                flags.bits.n = checkNegative(intMem & 0xFF);
                flags.bits.z = checkZero(intMem & 0xFF);
                flags.bits.c = bolRegACarry;

                _mem->setMEM((_byte_one + (_byte_two << 8)) + _X, ((unsigned char)(intMem & 0xFF)));

                // Update CPU Cycles
                _cpu_cycles = 7;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion

        #pragma region OpCode RTI
        // RTI - Return from Interrupt
        //  N Z C I D V
        //  _ _ _ _ _ _ <- From Stack
        case 0x40:  // RTI - Implied
            {
                int intPCAddr;

                // Store bits 4 and 5 because they are ignored on PLP stack PoP (http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior)
                bool tempBreak = flags.bits.b;
                bool tempBitFive = flags.bits.bit5;

                // Get Register Status from Stack
                flags.flags_byte = _mem->stackPop(_stack_pointer);

                // Restore bit 4 and 5
                flags.bits.b = tempBreak;
                flags.bits.bit5 = tempBitFive;

                // Get Program Counter Address from Stack
                intPCAddr = _mem->stackPop(_stack_pointer);
                intPCAddr += _mem->stackPop(_stack_pointer) << 8;

                // Copy new address to Program Counter
                _program_counter = intPCAddr;

                // Update CPU Cycles
                _cpu_cycles = 6;
            }
            break;
        #pragma endregion

        #pragma region OpCode RTS
        // RTS - Return from Subroutine
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x60:  // RTS - Implied
            {
                int intPCAddr;

                // Get Program Counter Address from Stack
                // 1 is added to the PC because the PC is always left in the last instruction(or last byte of an instruction set)
                // If you didn't add one, you would end up reading the last byte or instruction (that you've already read) instead
                // of moving on to the next instruction.  A good example is JSR, it pushed the program counter to the stack and the
                // location that is stored is that of Byte Two.  When we returne from that jump, we want to skip that last instruction
                // and move on to the next one.
                intPCAddr = _mem->stackPop(_stack_pointer) + 1;          
                intPCAddr += _mem->stackPop(_stack_pointer) << 8;

                // Copy new address to Program Counter
                _program_counter = intPCAddr;

                // Update CPU Cycles
                _cpu_cycles = 6;
            }
            break;
        #pragma endregion

        #pragma region OpCode SBC    (FIX) CHECK TO SEE IF THE FIRST OPERATION SHOULD BE UNSIGNED CHAR
        // SBC - Subtract Memory from Accumulator with Borrow with Carry
        //  N Z C I D V
        //  / / / _ _ /
        case 0xE9:  // SBC - Immediate
            {
                unsigned int tempA;

                // Add Accumulator - Byte Given - Carry
                tempA = (unsigned int)(_A - _byte_one - (1 - flags.bits.c));

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                // flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowSBC(_A, tempA, _byte_one);

                //if (tempA >= 0)
                //{
                //    flags.bits.c = true;
                //}
                //else if (tempA < 0)
                //{
                //    flags.bits.c = true;
                ////}

                //if (tempA > 0xFF)
                //{
                //    flags.bits.c = true;
                //}
                flags.bits.c = (tempA < 0x100);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xE5:  // SBC - Absolute Zero-Page
            {
                unsigned int tempA;

                // Add Accumulator - Byte Given - Carry
                tempA = (unsigned int)(_A - addrAbsZP(_byte_one) - (1 - flags.bits.c));

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                //flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowSBC(_A, tempA, addrAbsZP(_byte_one));

                //if (tempA > 0xFF)
                //{
                //    flags.bits.c = true;
                //}

                flags.bits.c = (tempA < 0x100);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xF5:  // SBC - Indexed Zero-Page (X)
            {
                unsigned int tempA;

                // Add Accumulator - Byte Given - Carry
                tempA = (unsigned int)(_A - addrIdxZP(_byte_one, _X) - (1 - flags.bits.c));

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                //flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowSBC(_A, tempA, addrIdxZP(_byte_one, _X));

                //if (tempA > 0xFF)
                //{
                //    flags.bits.c = true;
                //}

                flags.bits.c = (tempA < 0x100);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xED:  // SBC - Absolute
            {
                unsigned int tempA;

                // Add Accumulator - Byte Given - Carry
                tempA = (unsigned int)(_A - addrAbs(_byte_one, _byte_two) - (1 - flags.bits.c));

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                //flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowSBC(_A, tempA, addrAbs(_byte_one, _byte_two));

                //if (tempA > 0xFF)
                //{
                //    flags.bits.c = true;
                //}

                flags.bits.c = (tempA < 0x100);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xFD:  // SBC - Absolute, Indexed (X)
            {
                unsigned int tempA;

                // Add Accumulator - Byte Given - Carry
                tempA = (unsigned int)(_A - addrIdx(_byte_one, _byte_two, _X) - (1 - flags.bits.c));

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                //flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowSBC(_A, tempA, addrIdx(_byte_one, _byte_two, _X));

                //if (tempA > 0xFF)
                //{
                //    flags.bits.c = true;
                //}

                flags.bits.c = (tempA < 0x100);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xF9:  // SBC - Absolute, Indexed (Y)
            {
                unsigned int tempA;

                // Add Accumulator - Byte Given - Carry
                tempA = (unsigned int)(_A - addrIdx(_byte_one, _byte_two, _Y) - (1 - flags.bits.c));

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                //flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowSBC(_A, tempA, addrIdx(_byte_one, _byte_two, _Y));

                //if (tempA > 0xFF)
                //{
                //    flags.bits.c = true;
                //}

                flags.bits.c = (tempA < 0x100);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                
                _cpu_cycles = 4 + _page_changed_offset;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0xE1:  // SBC - Indirect (X)
            {
                unsigned int tempA;

                // Add Accumulator - Byte Given - Carry
                tempA = (unsigned int)(_A - addrIndX(_byte_one, _X) - (1 - flags.bits.c));

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                //flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowSBC(_A, tempA, addrIndX(_byte_one, _X));

                //if (tempA > 0xFF)
                //{
                //    flags.bits.c = true;
                //}

                flags.bits.c = (tempA < 0x100);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0xF1:  // SBC - Indirect (Y)
            {
                unsigned int tempA;

                // Add Accumulator - Byte Given - Carry
                tempA = (unsigned int)(_A - addrIndY(_byte_one, _Y) - (1 - flags.bits.c));

                // Set status bits
                flags.bits.n = checkNegative(tempA & 0xFF);
                flags.bits.z = checkZero(tempA & 0xFF);
                //flags.bits.c = checkCarry(tempA);
                flags.bits.v = checkOverflowSBC(_A, tempA, addrIndY(_byte_one, _Y));

                //if (tempA > 0xFF)
                //{
                //    flags.bits.c = true;
                //}

                flags.bits.c = (tempA < 0x100);

                _A = tempA & 0xFF;

                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 2;
            }
            break;
        #pragma endregion  

        #pragma region OpCode SEC
        // SEC - Set Carry Flag
        //  N Z C I D V
        //  _ _ 1 _ _ _
        case 0x38:  // SEC - Implied
            {
                // Set Carry Flag
                flags.bits.c = true;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode SED
        // SED - Clear Interrupt Disable Flag
        //  N Z C I D V
        //  _ _ _ _ 1 _
        case 0xF8:  // SED - Implied
            {
                // Set Decimal Mode Flag
                flags.bits.d = true;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode SEI
        // SEI - Set Interrupt Disable Flag
        //  N Z C I D V
        //  _ _ _ 1 _ _
        case 0x78:  // SEI - Implied
            {
                // Set Interrup Disable Flag
                flags.bits.i = true;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode STA
        // STA - Store Accumulator in Memory
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x85:  // STA - Absolute Zero-Page
            {
                // Store Accumulator in Memory
                _mem->setMEM(_byte_one, _A);

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x95:  // STA - Indexed Zero-Page (X)
            {
                // Store Accumulator in Memory+Offset
                _mem->setMEM((unsigned char)(_byte_one + _X), _A);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x8D:  // STA - Absolute
            {
                // Store Accumulator in Memory
                _mem->setMEM((_byte_one + (_byte_two << 8)), _A);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x9D:  // STA - Absolute, Indexed (X)
            {
                // Store Accumulator in Memory
                _mem->setMEM((_byte_one + (_byte_two << 8)) + _X, _A);

                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x99:  // STA - Absolute, Indexed (Y)
            {
                // Store Accumulator in Memory
                _mem->setMEM((_byte_one + (_byte_two << 8)) + _Y, _A);

                // Update CPU Cycles
                _cpu_cycles = 5;

                // Update Program Counter
                _program_counter += 3;
            }
            break;

        case 0x81:  // STA - Indirect (X)
            {
                // (FIX) -- WHATS UP WITH THE INT AND BYTE REFERENCES HERE
                int bLow, bHigh;
                unsigned char tLow = (unsigned char)(_byte_one + _X);

                // Get rLow and rHigh to find new address which 
                bLow = _mem->getMEM(tLow);    // Value from first Zero-Page Address
                bHigh = _mem->getMEM((unsigned char)(tLow + 1));    // Value from second Zero-Page Address

                // Store Accumulator in Memory
                _mem->setMEM(bLow + (bHigh << 8), _A);

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x91:  // STA - Indirect (Y)
            {
                unsigned char bLow, bHigh;

                // Get rLow and rHigh to find new address which 
                bLow = _mem->getMEM((int)(_byte_one));    // Value from first Zero-Page Address
                bHigh = _mem->getMEM((int)((unsigned char)(_byte_one + 1)));    // Value from second Zero-Page Address

                // Add Low Byte with X Offset after Getting Second Address (unlike addrIndX)
                //bLow += Y; (FIX) << -- THIS IS WHAT BROKE MARIO!!!!

                // Store Accumulator in Memory
                _mem->setMEM((bLow + (bHigh << 8)) + _Y, _A);

                // Update CPU Cycles
                _cpu_cycles = 6;

                // Update Program Counter
                _program_counter += 2;
            }
            break;
        #pragma endregion 

        #pragma region OpCode STX
        // STX - Store Register X in Memory
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x86:  // STX - Absolute Zero-Page
            {
                // Store Register X in Memory
                _mem->setMEM(_byte_one, _X);

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x96:  // STA - Indexed Zero-Page (Y)
            {
                // Store Register X in Memory+Offset
                _mem->setMEM((unsigned char)(_byte_one + _Y), _X);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x8E:  // STA - Absolute
            {
                // Store Register X in Memory
                _mem->setMEM((_byte_one + (_byte_two << 8)), _X);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion

        #pragma region OpCode STY
        // STY - Store Register X in Memory
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x84:  // STY - Absolute Zero-Page
            {
                // Store Register Y in Memory
                _mem->setMEM(_byte_one, _Y);

                // Update CPU Cycles
                _cpu_cycles = 3;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x94:  // STA - Indexed Zero-Page (X)
            {
                // Store Register Y in Memory+Offset
                _mem->setMEM((unsigned char)(_byte_one + _X), _Y);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 2;
            }
            break;

        case 0x8C:  // STA - Absolute
            {
                // Store Register Y in Memory
                _mem->setMEM((_byte_one + (_byte_two << 8)), _Y);

                // Update CPU Cycles
                _cpu_cycles = 4;

                // Update Program Counter
                _program_counter += 3;
            }
            break;
        #pragma endregion

        #pragma region OpCode TAX
        // TAX - Transfer Accumulator to Register X
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xAA:  // TAX - Implied
            {
                unsigned int tempA = _A;

                // Check Status Registers
                flags.bits.n = checkNegative(tempA);
                flags.bits.z = checkZero(_A);

                // Transfer Axxumulator to Register X
                _X = _A;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode TAY
        // TAY - Transfer Accumulator to Register Y
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xA8:  // TAY - Implied
            {
                unsigned int tempA = _A;

                // Check Status Registers
                flags.bits.n = checkNegative(tempA);
                flags.bits.z = checkZero(_A);

                // Transfer Axxumulator to Register X
                _Y = _A;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode TSX
        // TSX - Transfer Stack Pointer to Register X
        //  N Z C I D V
        //  / / _ _ _ _
        case 0xBA:  // TSX - Implied
            {
                unsigned int intRegSP = _stack_pointer;

                // Check Status Registers
                flags.bits.n = checkNegative(intRegSP);
                flags.bits.z = checkZero(_stack_pointer);

                // Transfer Stack Pointer to Register X
                _X = _stack_pointer;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode TXA
        // TXA - Transfer Register X to Accumulator
        //  N Z C I D V
        //  / / _ _ _ _
        case 0x8A:  // TXA - Implied
            {
                unsigned int intRegX = _X;

                // Check Status Registers
                flags.bits.n = checkNegative(intRegX);
                flags.bits.z = checkZero(_X);

                // Transfer Register X to Accumulator
                _A = _X;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode TXS
        // TXS - Transfer Register X to Stack Pointer
        //  N Z C I D V
        //  _ _ _ _ _ _
        case 0x9A:  // TXS - Implied
            {
                // Transfer Register X to Stack Pointer
                _stack_pointer = _X;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion

        #pragma region OpCode TYA
        // TYA - Transfer Register Y to Accumulator
        //  N Z C I D V
        //  / / _ _ _ _
        case 0x98:  // TXA - Implied
            {
                unsigned int intRegY = _Y;

                // Check Status Registers
                flags.bits.n = checkNegative(intRegY);
                flags.bits.z = checkZero(_Y);

                // Transfer Register Y to Accumulator
                _A = _Y;

                // Update CPU Cycles
                _cpu_cycles = 2;

                // Update Program Counter
                _program_counter += 1;
            }
            break;
        #pragma endregion 
        #pragma endregion

        // THIS SHOULD NOT HAPPEN
        default:
            {
                //// (FIX) This is a test &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& REMOVE ME !!!!!
                _program_counter += 1;
            }
            break;
    }

    // Probably going to let TinerasNES::run handle the (*15) stuff
    // intTotalCpuCycles += intOpCodeCpuCycles * 15;

    return _cpu_cycles;    //  Return value after executing OpCode
}
#pragma endregion

//    **********
//    NMI
#pragma region /* ----- NMI ----- */
int CPU::NMI()
{
    /* Several things happen when there is a Break
        1. Program Counter increases by 1 to bypass the current instuction on return
        2. The address in the Program Counter is PUSHed to the Stack
        3. Break flag is set
        4. Status Register is PUSHed to the Stack
        5. Interrupt Disable Flag is SET to 1
        6. Program Counter is Loaded with the default 'Break Address' 0xFFFE and 0xFFFF in this case
        7. Interrupt is handled by the code at that address
        8. Program Counter and Status Register are POPped from the Stack
        9. Control is returned to our program and it continues where it left off
     */

    // Increment Program Counter
    // registers.regPC++;

    // Push memory to stack
    _mem->stackPush(_stack_pointer, (unsigned char)(_program_counter >> 8)); // Push High Byte
    _mem->stackPush(_stack_pointer, (char)(_program_counter & 0xFF));    // Push Low Byte

    // Set Break Flag (REMEMBER TO DO THIS **AFTER** pushing SR to the stack so interrupt is cleared upon return
    // ?????? ?????? ?????? ?????? ????? ????????????????????? Reference (pg 25) says to set it before pushing SR ?????
    // WIKI says BEFORE, so I put it before http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
    flags.bits.b = false;
    //registers.statusBitFive = true;
    // Push Status Register to Stack
    _mem->stackPush(_stack_pointer, flags.flags_byte);




    // Set Interrupt flag (Side effect after pushing according to wiki - http://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior)
    flags.bits.i = true;
    // Set Program Counter to 0xFFFE and 0xFFFF to execute appropriate ISR
    /* This will get the Address of the next instructions to execute from
       Memory location 0xFFFE and 0xFFFF.  It should automatically be instructed to
       come back and continue where it left off after executing its ISR routine */
    _program_counter = _mem->getMEM(0xFFFA) | (_mem->getMEM(0xFFFB) << 8);

    // Update CPU Cycles
    _cpu_cycles = 7;

    // Reset all cycle specific values
    //cpuCycles = 0;
    //cpuCyclesOffset = 0;
    //pageChangedOffset = 0;

    return _cpu_cycles;    //  Return value after executing OpCode
}
#pragma endregion

//    **********
//    IRQ
#pragma region /* ----- IRQ ----- */
int CPU::IRQ()
{
    if(!flags.bits.i)
    {
        // Push memory to stack
        _mem->stackPush(_stack_pointer, (unsigned char)(_program_counter >> 8)); // Push High Byte
        _mem->stackPush(_stack_pointer, (char)(_program_counter & 0xFF));    // Push Low Byte

        flags.bits.b = false;

        _mem->stackPush(_stack_pointer, flags.flags_byte);

        flags.bits.i = true;

        _program_counter = _mem->getMEM(0xFFFE) | (_mem->getMEM(0xFFFF) << 8);

        // Update CPU Cycles
        _cpu_cycles = 7;

        // Reset all cycle specific values
        //cpuCycles = 0;
        //cpuCyclesOffset = 0;
        //pageChangedOffset = 0;

        return _cpu_cycles;    //  Return value after executing OpCode
    }

    return 0;
}
#pragma endregion


//    **********
//    Check Flags
#pragma region /* ----- Check Status Flag Bits ----- */
// Check to see if result had a Carry
bool CPU::checkCarry(int result)
{
    if (result >= 256)
    {
        return true;
    }
    return false;
}

// Check to see if result was Zero
bool CPU::checkZero(int result)
{
    if (result == 0)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

// Check for Overflow (ADC)
bool CPU::checkOverflowADC(char regA, unsigned int result, char charOne)
{
    /*The overflow flag is only set under two circumstances:
        1. There is a Carry from D6 to D7, but no Carry out of D7 (CF=0)
        2. There is a Carry from D7 out (CF=1), but no Carry from D6 to D7
        
        In other words, the overflow flag is set to 1 if there is a Carry from
        D6 to D7 or from D7 out, BUT NOT BOTH.
     */

    if ((((regA ^ charOne) & 0x80) == 0) && (((regA ^ result) & 0x80) != 0))
    {
        /* MessageBox.Show("True"); */
        return true;
    }
    else { /* MessageBox.Show("False");*/ return false; }
}

// Check for Overflow (SBC)
bool CPU::checkOverflowSBC(char regA, unsigned int result, char charOne)
{
    /*The overflow flag is only set under two circumstances:
        1. There is a Carry from D6 to D7, but no Carry out of D7 (CF=0)
        2. There is a Carry from D7 out (CF=1), but no Carry from D6 to D7
        
        In other words, the overflow flag is set to 1 if there is a Carry from
        D6 to D7 or from D7 out, BUT NOT BOTH.
     */

    if ((((regA ^ charOne) & 0x80) != 0) && (((regA ^ result) & 0x80) != 0))
    {
        /* MessageBox.Show("True"); */
        return true;
    }
    else { /* MessageBox.Show("False");*/ return false; }
}

// Check Sign
bool CPU::checkNegative(int result)
{
    if ((result & 0x80) == 0x80)
    {
        return true;
    }
    else { return false; }
}

// Check Sign
bool CPU::checkNegative(unsigned int result)
{
    if ((result & 0x80) == 0x80)
    {
        return true;
    }
    else { return false; }
}
#pragma endregion

//    **********
//    Page Change Check
#pragma region /* ----- Check for Page Crossing to update intCpuCyclesOffset ----- */
void CPU::pageChangeCheck(int newAddr)
{
    /* Checks to see if the memory page changes
       so that the number of CPU Cycles can be modified with the 
       correct value */

    // Compare previous address with the new one to see if page has changed
    if ((_previous_addrress & 0xFF00) == (newAddr & 0xFF00))
    {
        _page_changed_offset = 0;
    }
    else
    {
        _page_changed_offset = 1;
    }

    // Store new address in intLastAddress for comparison next time
    _previous_addrress = newAddr;
}
#pragma endregion

//    **********
//    Addressing Modes
#pragma region /* ----- Addressing Modes ----- */
unsigned char CPU::addrAbs(unsigned char bLow, unsigned char bHigh)  // Absolute Addressing Mode
{
    unsigned char result;

    pageChangeCheck(addHighLowRegXY(bLow, bHigh, NULL));

    // Add Low and High Bytes.  First need to shift High Byte by two Hex Places (16 * 16 = 256)
    result = _mem->getMEM(addHighLowRegXY(bLow, bHigh, NULL));
    
    return result;
}

unsigned char CPU::addrAbsZP(unsigned char bLow)  // Absolute[Zero-Page] Addressing Mode
{
    unsigned char result;

    pageChangeCheck(bLow);

    // Absolute[ZP] only reqiures Low Byte, High Byte is 0x00
    result = _mem->getMEM(bLow);
    
    return result;
}

unsigned char CPU::addrIdx(unsigned char bLow, unsigned char bHigh, unsigned char regXY)  // Indexed Addressing Mode
{
    unsigned char result;

    pageChangeCheck(addHighLowRegXY(bLow, bHigh, regXY));

    // Add Low and High Byte with X or Y Offset
    result = _mem->getMEM(addHighLowRegXY(bLow, bHigh, regXY));

    return result;
}

unsigned char CPU::addrIdxZP(unsigned char bLow, unsigned char regXY)  // Indexed[Zero-Page] Addressing Mode
{
    unsigned char result;

    pageChangeCheck((int)((unsigned char)(bLow + regXY)));

    // Add Low Byte with X or Y Offset, High Byte is 0x00
    result = _mem->getMEM((int)((unsigned char)(bLow + regXY)));

    return result;
}

int CPU::addrInd(unsigned char bLow, unsigned char bHigh)  // Indirect Addressing Mode (ONLY USED BY JMP OPCODE)
{
    /* In this mode, we are give an address that points to another address that is used with the JMP command.
       We get the low char from the address given, then increment that address and get the high char from the 
       next memory location.*/

    unsigned char rLow, rHigh;   // Low Byte Result, High Byte Result
    int result;         // Address attained after adding Low and High Result Bytes
        
    // Add Low Byte with X or Y Offset, High Byte is 0x00
    rLow = _mem->getMEM(addHighLowRegXY(bLow, bHigh, NULL));

    bLow += 1;      // Add 1 to get the value from the next address
    if (bLow == 0x00)   // If rLow value changed from 0xFF to 0x00 when adding 1, we add 1 to rHigh
    {
        bHigh += 1;
    }
    
    // Get High Byte from Next Memory Address
    rHigh = _mem->getMEM(addHighLowRegXY(bLow, bHigh, NULL));

    pageChangeCheck(addHighLowRegXY(rLow, rHigh, NULL));

    // Add the Low Result and High Result to get the new address to return
    result = addHighLowRegXY(rLow, rHigh, NULL);

    return result;
}

unsigned char CPU::addrIndX(unsigned char bLow, unsigned char regX)  // Pre-Indexed Indirect Addressing Mode (Only X Register & Only ZERO-PAGE)
{
    /* In this mode we add the contents of the X Register to the initial address given (hence 'Pre-Indexed') */

    unsigned char rLow, rHigh;   // Low Byte Result, High Byte Result
    unsigned char result;        // Address attained after adding Low and High Result Bytes

    // Add Low Byte with X Offset, High Byte is 0x00
    // Get rLow and rHigh to find new address which holds value we are looking for
    rLow = _mem->getMEM((unsigned char)(bLow + regX));    // Value from first Zero-Page Address
    rHigh = _mem->getMEM((int)((unsigned char)(bLow + regX + 1)));    // Value from second Zero-Page Address

    pageChangeCheck(addHighLowRegXY(rLow, rHigh, NULL));

    // Add the Low Result and High Result to get the new address of the desired value
    result = _mem->getMEM(addHighLowRegXY(rLow, rHigh, NULL));

    return result;
}

unsigned char CPU::addrIndY(unsigned char bLow, unsigned char regY)  // Post-Indexed Indirect Addressing Mode (Only Y Register & Only ZERO-PAGE)
{
    /* In this mode we add the contents of the X Register to the Indirect Address, not the given one ('Post-Indexed ) */

    unsigned char rLow, rHigh;   // Low Byte Result, High Byte Result
    unsigned char result;         // Address attained after adding Low and High Result Bytes

    int temp;

    // Get rLow and rHigh to find new address which holds value we are looking for
    rLow = _mem->getMEM(bLow);    // Value from first Zero-Page Address
    rHigh = _mem->getMEM((int)((unsigned char)(bLow + 1)));    // Value from second Zero-Page Address

    // Add Low Byte with X Offset after Getting Second Address (unlike addrIndX)
    // Must AND with 0xFFFF because the addresses rollover at 0xFFFF and you need
    // to get rid of the junk in the higher bits if the result is larger than 0xFFFF
    temp = addHighLowRegXY(rLow, rHigh, regY);

    pageChangeCheck(temp);

    // Add the Low Result and High Result to get the new address of the desired value
    result = _mem->getMEM(temp);

    return result;
}
#pragma endregion

//    **********
//    Set Compare Flags
#pragma region /* ----- Set Flags for CoMPare OpCodes (CMP, CPX, CPY) ----- */
void CPU::setCompareFlags(int regAXY, unsigned char byteOne)
{
    if (regAXY > byteOne)
    {
        flags.bits.c = true;
        flags.bits.z = false;
    }
    else if (regAXY == byteOne)
    {
        flags.bits.c = true;
        flags.bits.z = true;
    }
    else if (regAXY < byteOne)
    {
        flags.bits.c = false;
        flags.bits.z = false;
    }
}
#pragma endregion

//    **********
//    Add High Byte, Low Byte and RegXY
#pragma region /* ----- Add High Byte, Low Byte and RegXY ----- */
int CPU::addHighLowRegXY(unsigned char l, unsigned char h, unsigned char r)
{
    // Add High Byte to Low Byte, then add regXY, then & 0xFFFF
    return ((l + (h << 8)) + r) & 0xFFFF;
}
#pragma endregion

//    **********
//    CPU Constuctor
#pragma region /* ----- Write nestest file ----- */
void CPU::writeNestest()
{
    // Write filestream for nestest debug
    char streamTemp[60];
    //memset(streamTemp, 0, sizeof(streamTemp));
    //unsigned char tempP = 0xA5;//(char)flags.flagsByte;
    
    // Write exactly like nestest is formatted
    //sprintf(streamTemp, "%04X  %02X     A:%02X   X:%02X   Y:%02X   P:%02X   SP:%02X\n", PC, byteOpCode, A, X, Y, flags.flagsByte, SP);

    // Write like nestest, but with bytOne and byteTwo Operands
    sprintf(streamTemp, "%04X  %02X %02X %02X     A:%02X   X:%02X   Y:%02X   P:%02X   SP:%02X\n", _program_counter, _byte_op_code, _byte_one, _byte_two, _A, _X, _Y, flags.flags_byte, _stack_pointer);

    //nestestDebugStream << streamTemp;
}
#pragma endregion

//    **********
//    CPU Init Variables
#pragma region /* ----- CPU Init Variables ----- */
void CPU::init(MEM* mem)
{
    _mem = mem;

    reset(true);
}
#pragma endregion

//    **********
//    Reset CPU Variables
#pragma region /* ----- Reset CPU Variables ----- */
void CPU::reset(bool is_init)
{
    /******  *****  REGISTERS ***** ******/
    /* 6 Special and General Purpose Registers */
    _program_counter = 0;        // Program Counter
    _stack_pointer = 0xFD;    // Stack Pointer
    //P = 0;        // Status Register ('P'rocessor Status)
    _A = 0;        // Accumulator
    _X = 0;        // Index Register X
    _Y = 0;        // Index Register Y
    
    //opCodeCounter = 0;

    // Define Flags
    flags.bits.c = false;
    flags.bits.z = false;
    flags.bits.i = true;
    flags.bits.d = false;
    flags.bits.b = false;
    flags.bits.bit5 = true;
    flags.bits.v = false;
    flags.bits.n = false;

    if (is_init)
        IRQPending = false;
}
#pragma endregion

//    **********
//    CPU Constuctor
#pragma region /* ----- CPU Constructor ----- */
CPU::CPU()
{
}
#pragma endregion

//    **********
//    CPU Destructor
#pragma region /* ----- CPU Destructor ----- */
CPU::~CPU()
{
    // Close filestream for nestest debug
    //nestestDebugStream.close();
}
#pragma endregion
