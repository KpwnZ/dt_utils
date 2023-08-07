SRC  := $(wildcard src/*.c)
OBJS := $(SRC:%.c=%.o)

all: dt_utils

dt_utils: $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LOADLIBS) $(LDLIBS) -o $@

.PHONY: clean

clean:
	rm -rf dt_utils $(OBJS)
