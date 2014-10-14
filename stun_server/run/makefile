#C_FLAGS= -Wall -Wpointer-arith -O3 -pipe -g 
C_FLAGS= -Wall -g -DNDEBUG #-D__DEBUG
CXX = g++ 
AR = ar
#LIBDIR=/root/fyt_server
SOURCES_FILES_CPP=$(wildcard ./src/*.cpp)
OBJECTS = $(patsubst %.cpp,obj/%.o,$(notdir $(SOURCES_FILES_CPP)))

LIB_TARGET = lib/librun.a

INC= -I./include -I/usr/local/include

INC_DIR= include/
SRC_DIR= src/
OBJ_DIR= obj/
LIB_DIR= lib/

OBJ_EXT= .o
CXXSRC_EXT= .cpp

$(OBJ_DIR)%$(OBJ_EXT): $(SRC_DIR)%$(CXXSRC_EXT)
#	@echo
#	@echo "Compiling $< ==> $@"
#	@echo "mkdir -p $(OBJ_DIR) $(LIB_DIR)"
#	mkdir -p $(OBJ_DIR) $(LIB_DIR) 
	$(CXX) $(INC) $(C_FLAGS) -c $< -o $@

$(LIB_TARGET): $(OBJECTS)
#	@echo
#	@echo "$(OBJECTS) ==> $@"
	$(AR) rcs $(LIB_TARGET) $(OBJECTS)

all: $(LIB_TARGET)
clean:
	rm -f $(LIB_TARGET) $(OBJECTS)
