PLATFORM=$(shell uname)
CC = g++

BIN = xxtea
OBJS = main.o xxtea.o


all : $(BIN)

$(BIN) : $(OBJS)
		$(CC) -o $@ $^ $(LDFLAGS) 
clean :
	rm -f $(BIN) $(OBJS)
.PHONY : clean