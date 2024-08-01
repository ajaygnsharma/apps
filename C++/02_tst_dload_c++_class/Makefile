CXXFLAGS =	-O2 -g -Wall -fmessage-length=0
LDFLAGS = -ldl 

OBJS = main.o
LIBS = 

LIBSRC = triangle.cpp
SHLIB = libtriangle.so
TARGET = polygon

$(SHLIB): $(LIBSRC)
	 $(CXX) -g -shared -fPIC -o $(SHLIB) $(LIBSRC)
	
$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(LDFLAGS) main.cpp

lib: $(SHLIB)

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(SHLIB)
