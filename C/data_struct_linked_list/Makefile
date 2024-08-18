
TARGETS = $(TARGET1) $(TARGET2)

TARGET1 = llist
TARGET2 = fixed_sz_list

SRC1 = list.c list_test.c
SRC2 = fixed_sz_list.cpp 

LDFLAGS64=-lpthread -lboost_thread-mt -lboost_date_time -lboost_system-mt -luuid -lrt -lboost_serialization

#all: $(TARGET1) $(TARGET2)
all: $(TARGET1)

$(TARGET1): $(SRC1)
	gcc -Wall -g -o $@  $(SRC1)
	
$(TARGET2): $(SRC2)
	g++ -Wall $(LDFLAGS64) -g -o $@ $<
	
list_odd_numbers:
	gcc -Wall -g list_odd_numbers.c -o list_odd_numbers
	
clean:
	rm $(TARGET1) $(TARGET2) list_odd_numbers
