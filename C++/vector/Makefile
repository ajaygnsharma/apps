TARGET1 = int_vector
SRC1 = Intvector.cpp
TARGET2 = struct_vector
SRC2 = struct_vector.cpp


all: $(TARGET1) $(TARGET2)

$(TARGET1): 
	g++ -Wall -g $(SRC1) -o $(TARGET1)
	
$(TARGET2):	
	g++ -Wall -g $(SRC2) -o $(TARGET2)

clean:
	rm $(TARGET1) $(TARGET2)