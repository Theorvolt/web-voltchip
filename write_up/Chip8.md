

### A roadmap to implementing the emulator
Chip-8 is a very simple interpreted system, and its a menial task to implement the opcodes. That being said, the task is rather tedious in nature, and relies on implementing certain opcodes first. After the opcodes have been implemented, SDL is used to create the screen. The second-half of this project then utilises WebAssembly to transpile our code to JavaScript.



### Debugging Process

When testing the emulator, I utilised a test rom (link) to test each opcode. Of course, not every opcode was guaranteed to be functional, so I ensured that the current opcode and next opcode would be printed. This way, I could use a hex viewer (VSCode ext or HxD) to manually decipher the instruction and see what the intended behavior should be. For instance, I found that my program terminated early at the draw function, and I debugged it. 
