
TARGET =  tmpl_t inc_t alloc lba_calc struct_sz

all: $(TARGET)

tmpl_t:
	gcc -Wall -g main.c tmpl_struct.c -o $@
	
inc_t:	
	gcc -Wall -g incomplete_type.c -o $@

alloc:	
	gcc -Wall -g alloc.c -o $@
	
lba_calc:
	g++ -Wall -g lba_calc.cpp -o $@ 

struct_sz:
	g++ -O0 -Wall -g struct_sz.cpp -o $@

clean:
	-@rm $(TARGET) 2>/dev/null || true 