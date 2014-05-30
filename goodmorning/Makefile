CXXFLAGS =	-O0 -g -Wall -fmessage-length=0

OBJS =		goodmorning.o

LIBS = 	

TARGET =	goodmorning

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
