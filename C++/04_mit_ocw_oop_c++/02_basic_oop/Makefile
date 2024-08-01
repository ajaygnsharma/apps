CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -fPIC

OBJS =		Patient.o Virus.o

LIBS = 
SHLIB = libfoo.so
TARGET =	simulation

$(SHLIB): $(OBJS)
	 $(CXX) -g -shared -fPIC -o $(SHLIB) $(OBJS)
	
$(TARGET):	$(OBJS) $(SHLIB)
	$(CXX) -o $(TARGET) simulation.cc -L. -lfoo

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(SHLIB)
