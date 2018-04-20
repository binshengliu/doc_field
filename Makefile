
TARGET = dump_field

INDRI_PATH=/home/sl8/S3676608/.local

CXX = g++
CXXFLAGS = --std=c++11 -g -I$(INDRI_PATH)/include \
	-DHAVE_EXT_ATOMICITY=1 -DP_NEEDS_GNU_CXX_NAMESPACE=1
LDFLAGS = -L$(INDRI_PATH)/lib -lindri -lz -pthread

SRC = main.cpp
OBJS = $(SRC:.cpp=.o)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS)
