CXX = gcc
EMS = emcc
CXXFLAGS = -Wall -Werror $(SDLI) $(SDLL) -lSDL2
LDFLAGS = 

SRC = opcode.c volt.c main.c graphics.c
SRCDIR= src/
SRC_FP = $(addprefix $(SRCDIR),$(SRC))
OBJ = $(SRC_FP:.cc=.o)
EXEC = voltchip
SDLI = -I/usr/include/SDL2 
SDLL = -D_REENTRANT
W_OUT = app.html

# Add ROM files
R_INC = roms


all: $(EXEC)

# Local compilation
$(EXEC): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LBLIBS) $(CXXFLAGS)

# Deploy for web
wasm:
	$(EMS) -o $(W_OUT) $(OBJ) -Wall -Werror -s USE_SDL=2 --embed-file $(R_INC)

clean:
	rm -rf $(OBJ) $(EXEC)