CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		main.o MITPerson.o

LIBS =

TARGET =	08_copy_cnstrctr

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
