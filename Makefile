TARGET = bin/test
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*

$(TARGET): $(OBJ)
	cc -O0 -g3 -o $@ $?

obj/%.o : src/%.c
	cc -O0 -g3 -c $< -o $@ -Iinclude

test:
	$(TARGET) array_example.json
