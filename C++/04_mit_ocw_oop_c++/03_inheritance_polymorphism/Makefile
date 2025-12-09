CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		Class.o MITPerson.o Student.o main.o

LIBS =

TARGET =	mit_lec05

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
