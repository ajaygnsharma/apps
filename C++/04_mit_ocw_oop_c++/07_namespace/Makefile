CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		07_namespace.o

LIBS =

TARGET =	07_namespace

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
