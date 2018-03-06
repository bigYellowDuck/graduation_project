CXX = g++

CXXFLAGS = -O2 -std=c++11

LDFFLAGS = -lev -pthread

PROJ_PATH = $(shell pwd|awk -F '/proxy' '{print $$1}')/proxy

PROJ_SRC_PATH = $(PROJ_PATH)/src

PROJ_INC_PATH = $(PROJ_PATH)/include

PROC_LIB_PATH = $(PROJ_PATH)/lib

LIBEV_PATH = $(PROJ_INC_PATH)/libev-4.15
LIBEV_INC_PATH = $(LIBEV_PATH)
LIBEV_SO = libev.so.4.0.0

RAPIDJSON_PATH = $(PROJ_INC_PATH)/rapidjson
RAPIDJSON_INC_PATH = $(RAPIDJSON_PATH)/include

PROJ_INC = -I$(PROJ_SRC_PATH) -I$(LIBEV_INC_PATH) -I$(RAPIDJSON_INC_PATH)

PROC_LINK = -L$(PROC_LIB_PATH)

SOURCES += ${shell find '$(PROJ_SRC_PATH)' -name '*.cpp'}

OBJECTS = ${SOURCES:.cpp=.o}

TARGET = server

$(TARGET) : $(OBJECTS)
	$(CXX) -o $@ $^ $(PROJ_INC) $(CXXFLAGS) $(PROC_LINK) $(LDFFLAGS)

$(OBJECTS) : %.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $(PROJ_INC) $< -o $@


.PHONY : clean
clean:
	rm $(TARGET) $(OBJECTS)

