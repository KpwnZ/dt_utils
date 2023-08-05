SRC    := src/list.c src/dt.c src/main.c
LDLIBS := 

all: dt_utils

dt_utils: $(SRC:%.c=%.o)
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	$(RM) dt_utils $(SRC:%.c=%.o)

