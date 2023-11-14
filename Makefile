CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

AS_SRC_FILES = AS.c
USER_SRC_FILES = user.c

AS_OBJ_FILES = $(AS_SRC_FILES:.c=.o)
USER_OBJ_FILES = $(USER_SRC_FILES:.c=.o)

AS_EXECUTABLE = AS
USER_EXECUTABLE = user

all: $(AS_EXECUTABLE) $(USER_EXECUTABLE)

$(AS_EXECUTABLE): $(AS_OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

$(USER_EXECUTABLE): $(USER_OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(AS_EXECUTABLE) $(USER_EXECUTABLE)

.PHONY: all clean