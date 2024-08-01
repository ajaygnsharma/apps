CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		function_table.o

LIBS =

TARGET =	function_table

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
