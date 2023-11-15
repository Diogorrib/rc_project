CC = gcc
CFLAGS = -std=c17 -D_POSIX_C_SOURCE=200809L
# Warnings
CFLAGS += -fdiagnostics-color=always -Wall -Werror -Wextra -Wcast-align -Wconversion -Wfloat-equal -Wformat=2 -Wnull-dereference -Wshadow -Wsign-conversion -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused
# Warning suppressions
CFLAGS += -Wno-sign-compare

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

# optional O3 optimization symbols: run make OPTIM=no to deactivate them
ifeq ($(strip $(OPTIM)), no)
	CFLAGS += -O0
else
	CFLAGS += -O3
endif

# optional debug symbols: run make DEBUG=no to deactivate them
ifneq ($(strip $(DEBUG)), no)
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
