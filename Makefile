OUT=mmSim
SOURCE=sim.c mouse.c
COMPILER=gcc
LIB=ncurses

all:	$(OUT)

$(OUT):$(SOURCE)
	$(COMPILER) -o $(OUT) $(SOURCE) -l$(LIB)
