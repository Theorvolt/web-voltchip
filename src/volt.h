#define RAM_SIZE 0x1000 // May as well work in hex
#define WIDTH 32
#define LENGTH 64
#define KEY_NUM 16
#define STACK_SIZE 16
#define REG_NUM 16
#define MAX_ROMSIZE 0xE00

typedef struct interpreter {
    // unsigned char = 1 byte
    unsigned char Vreg[REG_NUM], Sound, Delay, SP;  
    // unsigned short = 2 bytes
    unsigned short PC, Ireg; 
    unsigned short Stack[STACK_SIZE]; // Each is two bytes (16 bits)
    // unsigned int = 8 bytes
    unsigned char display[LENGTH*WIDTH]; // Cool idea: Implement a bitset, I've already got an implementation in python.
    unsigned char memory[RAM_SIZE]; // char = two hex characters, e.g FF or E2. So two slots are required to decode one opcdoe/
    unsigned char key[KEY_NUM];
    unsigned char dFlag;
} interpreter;

void clearScreen(interpreter *ip);
void startup(interpreter *ip);
void runCycle(interpreter *ip);