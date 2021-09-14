PLATFORM=$(shell uname)
CC = gcc

BIN = test
OBJS = main.o rb_tree.o


all : $(BIN)

$(BIN) : $(OBJS)
		$(CC) -o $@ $^ $(LDFLAGS) 
clean :
	rm -f $(BIN) $(OBJS)
.PHONY : clean