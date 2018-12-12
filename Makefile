CC = mpicc
CFLAGS = -g -Wall

PROGRAMS = master map reduce

all: $(PROGRAMS)

% : %.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY : clean

clean:
	rm *~ $(PROGRAMS)

