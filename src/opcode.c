#include "volt.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// 00EE - Return from function
void RET(interpreter *ip) {
    ip->SP--;
    ip->PC = ip->Stack[ip->SP]; // Set PC to stored address
    ip->PC += 2;
}

// 00E0 - Clear screen
void CLS(interpreter *ip) {
    clearScreen(ip);

    ip->dFlag = 1;
    ip->PC += 2;

    sleep(5);

}

// 1nnn - Jump to address
void JUMP(interpreter *ip, int addr) {
    ip->PC = addr;
}

// 2nnn - call function (subroutine)
void FUNC(interpreter *ip, int addr) {
    ip->Stack[ip->SP] = ip->PC;
    ip->SP++;
    ip->PC = addr;
}

// 3xkk - if V_X == val: skip
void SKIP_EQ(interpreter *ip, int reg1, int val) {
    if (ip->Vreg[reg1]==val) {

        ip->PC+=4;
    }
    else {

        ip->PC+=2;
    }
}

// 4xkk - if V_X != val: skip
void SKIP_NEQ(interpreter *ip, int reg1, int val) {

    if (ip->Vreg[reg1]!=val) {
        ip->PC+=4;
    }
    else {
        ip->PC+=2;
    }
}

// 5xy0 - if V_X == V_Y: skip
void SKIPR_EQ(interpreter *ip, int reg1, int reg2) {
    if (ip->Vreg[reg1]==ip->Vreg[reg2]) {
        ip->PC+=4;
    }
    else {
        ip->PC+=2;
    }
}

// 6xkk - V_X = val
void LD_VAL(interpreter *ip, int reg1, int val) {
    ip->Vreg[reg1] = val;
    ip->PC+=2;
}

// 7xkk - V_X += val
void ADD_VAL(interpreter *ip, int reg1, int val) {
    ip->Vreg[reg1] += val;
    ip->PC+=2;
}

// 8xy0 - V_X = V_Y
void LD_REG(interpreter *ip, int reg1, int reg2) {
    ip->Vreg[reg1] = ip->Vreg[reg2];
    ip->PC+=2;
}

// 8xy1 - V_X |= V_Y
void OR(interpreter *ip, int reg1, int reg2) {

    ip->Vreg[reg1] |= ip->Vreg[reg2];

    ip->PC+=2;


}

// 8xy2 - V_X &= V_Y
void AND(interpreter *ip, int reg1, int reg2) {
    ip->Vreg[reg1] &= ip->Vreg[reg2];
    ip->PC+=2;
}

// 8xy3 - V_X ^= V_Y
void XOR(interpreter *ip, int reg1, int reg2) {
    ip->Vreg[reg1] ^= ip->Vreg[reg2];
    ip->PC+=2;
}

// 8xy4 - V_X += V_Y; carry
void ADD_CARRY(interpreter *ip, int reg1, int reg2) {
    ip->Vreg[reg1] += ip->Vreg[reg2];

    if ((int)ip->Vreg[reg2]+(int)ip->Vreg[reg1] < 0xFF) { // V_X+V_Y > 256 ? 1 : 0
        ip->Vreg[0xF] &= 0;
    }
    else {
        ip->Vreg[0xF] = 1;
    }
    ip->PC+=2;
}

// 8xy5 - V_X -= V_Y; account for negative
void SUB_CARRY(interpreter *ip, int reg1, int reg2) {

    if ((int)ip->Vreg[reg1] - (int)ip->Vreg[reg2] >= 0) {
        ip->Vreg[0xF] = 1; // Borrow
    }
    else {
        ip->Vreg[0xF] &= 0; // None
    }
    ip->Vreg[reg1] -= ip->Vreg[reg2];

    ip->PC+=2;
}

// 8xy6 - V_X >>= 1
void SHR(interpreter *ip, int reg1) {
    ip->Vreg[0xF] = ip->Vreg[reg1] & 0x1;
    ip->Vreg[reg1] >>= 1;
    ip->PC+=2;
}

// 8xy7 
void SUB_NEG(interpreter *ip, int reg1, int reg2) {
    if (ip->Vreg[reg2] > ip->Vreg[reg1]) {
        ip->Vreg[0xF] = 1;
    }
    else {
        ip->Vreg[0xF] = 0;
    }
    ip->Vreg[reg1] = ip->Vreg[reg2]-ip->Vreg[reg1];
    ip->PC+=2;
}

// 8xyE - V_X <<= 1
void SHL(interpreter *ip, int reg1) {
    ip->Vreg[0xF] = ip->Vreg[reg1] >> 7;
    ip->Vreg[reg1] <<= 1;
    ip->PC+=2;
}

// 9xy0 - if V_X != V_Y: skip
void SKIPR_NEQ(interpreter *ip, int reg1, int reg2) {
    if (ip->Vreg[reg1] != ip->Vreg[reg2]) {
        ip->PC+=4;   
    }
    else {
        ip->PC+=2;
    }
}

// Annn - I = addr
void LOAD_I(interpreter *ip,int address) {
    ip->Ireg = address;
    ip->PC+=2;
}

// Bnnn - PC = V_0+addr
void JUMP_V0(interpreter *ip,int address) {
    ip->PC = address+ip->Vreg[0x0];
}

// Cxkk - V_X = (rng() % 256) & val
void RND(interpreter *ip, int reg1, int val) {
    ip->Vreg[reg1] = (rand() % 0x100) & (val);
    ip->PC+=2;
}

// Dxyn - Update internal rep of screen
void DRAW(interpreter *ip, int reg1, int reg2, int nibble) {
    ip->Vreg[0xF] = 0;
    unsigned short p;
    int xval = ip->Vreg[reg1];
    int yval = ip->Vreg[reg2];
    for (int y = 0; y < nibble; y++) {
        p = ip->memory[ip->Ireg+y];
        for (int x = 0; x < 8; x++) {

                if ((p & (0x80 >> x)) != 0) {
                    if (ip->display[xval+x+(y+yval)*64]==1)
                    {
                        ip->Vreg[0xF] = 1;
                    }
                    ip->display[xval+x+(y+yval)*64] ^= 1;
                }
        }
    }
    // Update to screen
    
    ip->dFlag = 1;
    ip->PC+=2;
}

// Ex9E - If key is pressed: skip
void SKIP_KEY(interpreter *ip, int reg1) {
    if (ip->key[ip->Vreg[reg1]] != 0) {
        ip->PC+=4;
    }
    else {
        ip->PC+=2;
    }
}

// ExA1 - If key is not pressed: skip
void SKIPN_KEY(interpreter *ip, int reg1) {
    if (ip->key[ip->Vreg[reg1]] == 0) {
        ip->PC+=4;
    }
    else {
        ip->PC+=2;
    }
}

// Fx07 - V_X = Delay
void LD_DT(interpreter *ip, int reg1) {
    ip->Vreg[reg1] = ip->Delay;
    ip->PC+=2;
}

// FX0A - V_0...V_N = Key_presses;
void LD_KEY(interpreter *ip, int reg1) {
    unsigned char key_press = 0;
    for (int index=0; index<KEY_NUM; ++index) {
        if (ip->key[index] != 0) {
            ip->Vreg[reg1] = index; // set to key pressed (first)
            key_press = 1;
        }
    }
    if (!key_press) {
        return; // Break out without PC increment
    }
    ip->PC+=2;
}

// Fx15 - Delay = V_X
void ST_DT(interpreter *ip, int reg1) {
    ip->Delay = ip->Vreg[reg1];
    ip->PC+=2;
}

// Fx18 - Sound = V_X
void LD_SD(interpreter *ip, int reg1) {
    ip->Sound = ip->Vreg[reg1];
    ip->PC+=2;
}

// Fx1E - I += V_X
void ADD_I(interpreter *ip, int reg1) {
    if (ip->Ireg +ip->Vreg[reg1] > 0xFFF) {
        ip->Vreg[0xF] = 1;
    }
    else {
        ip->Vreg[0xF] = 0;
    }
    ip->Ireg += ip->Vreg[reg1];
    ip->PC+=2;
}

// Fx29 
void LD_DIGIT(interpreter *ip, int reg1) {
    ip->Ireg = ip->Vreg[reg1] * 0x5;
    ip->PC+=2;
}

// Fx33
void LD_V(interpreter *ip, int reg1) {
    ip->memory[ip->Ireg] = ip->Vreg[reg1]/100; // 100s digits
    ip->memory[ip->Ireg+1] = (ip->Vreg[reg1]/10)%10; // 10s digits
    ip->memory[ip->Ireg+2] = ip->Vreg[reg1]%10; // units
    ip->PC+=2;
}

// Fx55
void ST_V(interpreter *ip, int reg1) {
    for (int pos = 0; pos <= reg1; ++pos) {
        ip->memory[ip->Ireg+pos] = ip->Vreg[pos];
    }
    ip->Ireg += reg1 +1; // Rebase I
    ip->PC+=2;
}

// Fx65
void LD_VI(interpreter *ip, int reg1) {
    for (int pos = 0; pos <= reg1; ++pos) {
        ip->Vreg[pos] = ip->memory[ip->Ireg+pos];
    }
    ip->Ireg += reg1 +1; // Rebase I
    ip->PC+=2;
}