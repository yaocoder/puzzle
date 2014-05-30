CXXFLAGS =	-O0 -g -Wall -fmessage-length=0

OBJS =		accuser.o

LIBS =

TARGET =	accuser

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
