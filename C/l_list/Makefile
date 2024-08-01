
TARGETS = $(TARGET1) $(TARGET2)

TARGET1 = llist
TARGET2 = fixed_sz_list

SRC1 = list.c list_test.c
SRC2 = fixed_sz_list.cpp 

LDFLAGS64=-lpthread -lboost_thread-mt -lboost_date_time -lboost_system-mt -luuid -lrt -lboost_serialization

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(SRC1)
	gcc -Wall -g -o $@  $(SRC1)
	
$(TARGET2): $(SRC2)
	g++ -Wall $(LDFLAGS64) -g -o $@ $<
	
clean:
	rm $(TARGET1) $(TARGET2)
