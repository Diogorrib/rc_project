CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

INCLUDE_DIRS := src/user src/server src/
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

TARGETS = src/user/user src/server/server
TARGET_EXECS = user AS

USER_SOURCES := $(wildcard src/user/*.c)
COMMON_SOURCES := $(wildcard src/common/*.c)
SERVER_SOURCES := $(wildcard src/server/*.c)
SOURCES := $(USER_SOURCES) $(COMMON_SOURCES) $(SERVER_SOURCES)

USER_HEADERS := $(wildcard src/user/*.h)
COMMON_HEADERS := $(wildcard src/common/*.h)
SERVER_HEADERS := $(wildcard src/server/*.h)
HEADERS := $(USER_HEADERS) $(COMMON_HEADERS) $(SERVER_HEADERS)

USER_OBJECTS := $(USER_SOURCES:.c=.o)
COMMON_OBJECTS := $(COMMON_SOURCES:.c=.o)
SERVER_OBJECTS := $(SERVER_SOURCES:.c=.o)
OBJECTS := $(USER_OBJECTS) $(COMMON_OBJECTS) $(SERVER_OBJECTS)

CFLAGS += $(INCLUDES)

vpath # clears VPATH
vpath %.h $(INCLUDE_DIRS)

# Run `make OPTIM=no` to disable -O3
ifeq ($(strip $(OPTIM)), no)
	CFLAGS += -O0
else
	CFLAGS += -O3
endif

# Run `make DEBUG=true` to run with debug symbols
ifeq ($(strip $(DEBUG)), yes)
	CFLAGS += -g
endif


.PHONY: all clean fmt fmt-check package

all: $(TARGET_EXECS)

fmt: $(SOURCES) $(HEADERS)
	clang-format -i $^

fmt-check: $(SOURCES) $(HEADERS)
	clang-format -n --Werror $^

src/server/server: $(SERVER_OBJECTS) $(SERVER_HEADERS) $(COMMON_OBJECTS) $(COMMON_HEADERS)
src/user/user: $(USER_OBJECTS) $(USER_HEADERS) $(COMMON_OBJECTS) $(COMMON_HEADERS)

AS: src/server/server
	cp src/server/server AS
user: src/user/user
	cp src/user/user user

clean:
	rm -f $(OBJECTS) $(TARGETS) $(TARGET_EXECS)

clean-data:
	rm -rf .txt
