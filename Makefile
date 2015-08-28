CXX   = g++
STDCCFLAGS=-I./
LFLAG=-lm -lpthread -lz -lrt

CFLAGS  = -W -Wall -g -D_POSIX_PTHREAD_SEMANTICS -D_REENTRANT $(STDCCFLAGS)
TARGET=rcsgrep

SOURCES := \
           AxByteArray.cpp \
           AxString.cpp \
           AxRecordSet.cpp \
           AxUtil.cpp \
           TypeDef.cpp \
           Logger.cpp \
			  LLTParser.cpp \
           Condition.cpp \
			  SessionMap.cpp \
			  Fractionator.cpp \
			  Printer.cpp \
           main.cpp

CLEAN_FILES += $(OBJS)
CLEAN_FILES += $(TARGET)

######################################################################
# create list of object files 
SOURCES  := $(strip $(SOURCES))
SRC_OBJS := $(SOURCES:.c=.o)
SRC_OBJS := $(SRC_OBJS:.cpp=.o)
OBJS     := $(patsubst %.o, %.o, $(notdir $(SRC_OBJS) $(OBJS)))

all : $(TARGET)

$(OBJS) :
%.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@


$(TARGET) : $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LFLAG) 

clean :
	rm -rf $(CLEAN_FILES) 

