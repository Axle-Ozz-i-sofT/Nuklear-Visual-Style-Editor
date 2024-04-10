# Install
BIN = nuklear-visual-style-editor

# Flags
CFLAGS += -std=c99 -Wall -Wextra -pedantic -Wno-unused-function -O2

SRC = main.c
OBJ = $(SRC:.c=.o)

# TODO: Handle Windows build
#ifeq ($(OS),Windows_NT)
#BIN := $(BIN).exe
#LIBS = -lallegro -lallegro_main -lallegro_image -lallegro_font \
	-lallegro_ttf -lallegro_primitives -lallegro_dialog -lm
#else
LIBS = -lallegro -lallegro_main -lallegro_image -lallegro_font \
	-lallegro_ttf -lallegro_primitives -lallegro_dialog -lm
#endif

$(BIN):
	rm -f resources/$(BIN) $(OBJS)
	$(CC) $(SRC) $(CFLAGS) -o resources/$(BIN) $(LIBS)

clean:
	rm -f resources/$(BIN) $(OBJ)