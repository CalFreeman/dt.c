CC = gcc
CFLAGS = -g
TARGET = dt
OBJS = dt.c

$(TARGET): $(OBJS)
        $(CC) -o $(TARGET) $(OBJS)

dt.o: dt.c
        $(CC) $(CFLAGS) -c dt.c

clean:
        /bin/rm -f *.o $(TARGET)