#include <stdlib.h>
#include <stdio.h>
#include "volt.h"
#include "opcode.h"

// First order of business, memory allocation.
/*
4kb memory (4096 bytes)


------ 512 (System reserved below)

16 registers, 1 byte each 
V1 .... VF (15) and then I
= 16 bytes

2 special 1 byte pointers
= 2 bytes
*/

/*fontset next
5*16 = 80
Access for one character would be fontset[a:a+5] (roughly)
*/
unsigned int fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // Hex form of 1 
    0x20, 0x60, 0x20, 0x20, 0x70, // 2
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x90, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80 // F
};

/*
First 512 bytes:
(V0...F): 16 primary registers, 1 byte each 
(I): 1 register, 1 byte 
(Sound and Delay): 2 registers, 1 byte each 
(PC): 1 reg, 2 bytes
(SP): 1 reg, 1 byte 
(Stack): 16*2 bytes = 32 bytes
TOTAL = 54 bytes

Display:
64 by 32 bitmap -> 2048 positions, but monochrome so really just 2048/8 = 256 bytes.

Font:
Each is 5 bytes. 16 characters -> 16*5 = 80 bytes.
*/


void startup(interpreter *ip) {
    // Init interpreter objects
    
    ip->PC = 0x200;
    ip->Ireg = 0;
    ip->Sound = 0;
    ip->Delay = 0;
    ip->SP = 0;
    ip->dFlag = 0;
    for (int val = 0; val<REG_NUM; ++val) {
        ip->Stack[val] = 0;
        ip->Vreg[val] = 0;
        ip->key[val] = 0;
    }
    for (int val = 0; val < RAM_SIZE; ++val) {
        ip->memory[val] = 0;
    }
    for(int i = 0; i < 80; ++i)
        ip->memory[i] = fontset[i]; // load fontset into memory
    clearScreen(ip);
} 

void clearScreen(interpreter *ip) {
    for (int pos = 0; pos < LENGTH*WIDTH; ++pos) {
        ip->display[pos]=0;
}
    ip->dFlag = 1;
}

void runCycle(interpreter *ip) {
    // Fetch next opcode in memory. Each opcode is 2 chars, so shift 8 bits to bitwise with the next part.
    // e.g [FF,A8] -> 0xFFA8
    
    int cPC = ip->PC;
    int opcode = ip->memory[cPC] << 8 | ip->memory[cPC+1];

    // Worthwhile precomputing values, that way code does not need to be reused
    int address = opcode & 0x0FFF; // get last three chars
    int reg1 = (opcode & 0x0F00) >> 8; // get second char
    int reg2 = (opcode & 0x00F0) >> 4; // get third char
    int val = opcode & 0x00FF;
    int nibble = opcode & 0x000F;
    //printf("Opcode: %x | PC: %x | Next: %x%x %x%x \n",opcode,cPC,ip->memory[cPC+2],ip->memory[cPC+3],ip->memory[cPC+4],ip->memory[cPC+5]);
    //printf("Chip off: %x\n",opcode & 0xF000);
    // Decode, bitwise & against F results in yielding the character
    switch (opcode & 0xF000) {
        case 0x0000:
            /*
            00E0 - Clear screen
            00EE - return from subroutine
            */
           switch (val) { // Match by last char
                case 0x0E0: // CLS, i have messed this up 3 times...
                    CLS(ip);
                    break;
                case 0x0EE: // RET
                    RET(ip);
                    break;
                default:
                    printf("Unknown opcode %x provided",opcode);
                    exit(69420);
           }
            break;
        case 0x1000:
            //1nnn - JUMP nnn
            JUMP(ip, address);
            break;
        case 0x2000:
            FUNC(ip,address);
            break;
        case 0x3000:
            // 3xkk - IF Vx == kk; skip
            SKIP_EQ(ip,reg1,val);
            break;
        case 0x4000:
            // 4xkk - IF Vx != kk; skip inst
            SKIP_NEQ(ip,reg1,val);   
            break;
        case 0x5000:
            // 5xkk - IF Vx == Vy; skip inst
            SKIPR_EQ(ip,reg1,reg2);
            break;
        case 0x6000:
            // 6xkk - LOAD Vx, kk
            LD_VAL(ip,reg1,val);
            break;
        case 0x7000:
            // 7xkk - ADD Vx, kk
            ADD_VAL(ip,reg1,val);
            break;
        case 0x8000:
           switch (nibble) {
               case 0x0000:
                    LD_REG(ip,reg1,reg2);
                    
                    break;

               case 0x0001:
                    OR(ip,reg1,reg2);
                    break;

               case 0x0002:
                    AND(ip,reg1,reg2);
                    break;

               case 0x0003:
                    XOR(ip,reg1,reg2);
                    break;

               case 0x0004:
                    ADD_CARRY(ip,reg1,reg2);
                    break;

               case 0x0005:
                    SUB_CARRY(ip,reg1,reg2);
                    break;

               case 0x0006:
                    SHR(ip,reg1);
                    break;

               case 0x0007:
                    SUB_NEG(ip,reg1,reg2);
                    break;

               case 0x000E:
                    SHL(ip,reg1);
                    break;
                default:
                    printf("Unknown opcode %x provided",opcode);
                    exit(69420);
           }
           break;

        case 0x9000:
        // 9xy0 - if Vx!=Vy; skip
            SKIPR_NEQ(ip,reg1,reg2);
            break;
        case 0xA000:
        // Annn - I = nnn
            LOAD_I(ip,address);
            break;
        case 0xB000:
        // Bnnn - JUMP nnn+v0
            JUMP_V0(ip,address);
            break;
        case 0xC000:
        // Cxkk - RND, Vx, byte
            RND(ip,reg1,val);
            break;

        case 0xD000:
        // Dxyn - Draw, Vx, Vy, nibble
            DRAW(ip,reg1,reg2,nibble);
            break;

        case 0xE000:
            switch (val) { // check nibble for value
                case 0x09E: 
                    // Ex9E - SKIP Vx | if (Key[Vreg[x]]!=0); skip
                    SKIP_KEY(ip,reg1);
                    break;
                case 0x0A1:
                    // ExA1 - SKIP Vx | if (Key[Vreg[x]]==0); skip
                    SKIPN_KEY(ip,reg1);
                    break;
                default:
                    printf("Unknown opcode %x provided",opcode);
                    exit(1);
            }
            break;
        case 0xF000:
            switch (val) {
                case 0x0007:
                    // Fx07 - LD, Vx, DT
                    LD_DT(ip,reg1);
                    break;
                
                case 0x000A:
                    // Check for keypress
                    LD_KEY(ip,reg1);
                    break;
                
                case 0x0015:
                    // Fx15 - LD, DT, Vx
                    ST_DT(ip,reg1);
                    break;
                
                case 0x0018:
                    // Fx18 - LD, ST, Vx
                    LD_SD(ip,reg1);
                    break;
                
                case 0x001E:
                    // Fx1E - ADD, I, Vx
                    ADD_I(ip,reg1);
                    break;
                
                case 0x0029:
                    // I = address of the sprite for char Vx
                    LD_DIGIT(ip,reg1);
                    break;
                
                case 0x0033:
                    // FX33 - Stores value at Vx in address I,I+1,I+2
                    LD_V(ip,reg1);
                    break;
                
                case 0x0055:
                    // FX55 - LD [I], Vx | stores V0-Vx in memory starting at I
                    ST_V(ip,reg1);
                    break;
                
                case 0x0065:
                    // Fx65 - LD, Vx, [I] | Does the other of above
                    LD_VI(ip,reg1);
                    break;

                default:
                    printf("Unknown opcode %x provided",opcode);
                    exit(69420);
            }
            break;

            default:
                printf("Unknown opcode %x provided",opcode);
                exit(69420);
    }

    
    // Update Sound and Delay
    if (ip->Delay>0) {
        ip->Delay--;
    }
    if (ip->Sound>0) {
        ip->Sound--;
    }
}