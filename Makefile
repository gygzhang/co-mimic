DESTINATION := main
LIBS :=
INCLUDES := .


RM := rm -f
#C,CC或CPP文件的后缀
suffix=cpp
# GNU Make的隐含变量定义
CC=g++
CPPFLAGS = -g -Wall -O0 -march=native -std=c++17 -Wno-reorder -pthread
CPPFLAGS += $(addprefix -I,$(INCLUDES)) 
#CPPFLAGS += -MMD

# match all *.cpp files
SOURCE := $(wildcard *.$(suffix)) 
OBJS := $(patsubst %.$(suffix),%.o,$(SOURCE))
DEPS := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.$(suffix), $(MISSING_DEPS)))

.PHONY : all deps objs clean rebuild

all : $(DESTINATION)

deps : $(DEPS)
		$(CC) -MM -MMD $(SOURCE) 


objs : $(OBJS)

clean :
		@$(RM) *.o
		@$(RM) *.d
		@$(RM) $(DESTINATION)

rebuild: clean all 

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
		@$(RM) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

$(DESTINATION) : $(OBJS)
		$(CC) -o $(DESTINATION) $(OBJS) $(addprefix -l,$(LIBS)) $(CPPFLAGS)
