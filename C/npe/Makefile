OBJS=5_30_c.o ne_tst_01.o

all: $(OBJS)

.c: .o
	gcc -Wall -o 

5_30_c: 5_30_c.o
	gcc $^ -o $@

5_30_c.o: 5_30_c.c 
	gcc -c $<


#^ is the name of the prerequisites: the name after : 
#@ is the name of the target
#< is the name of the first pre requisite. The first filename after:

