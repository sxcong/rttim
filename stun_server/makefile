#C_FLAGS= -Wall -Wpointer-arith -O3 -pipe -g 
C_FLAGS= -Wall -g -DNDEBUG
CXX= g++ 

INC=   -I./run/include  -I. 

LIB= -L./run/lib -lrun  -lpthread 

SOURCES_FILES_CPP=$(wildcard *.cpp)
EXE_FILE=$(SOURCES_FILES_CPP:%.cpp=%)

$(EXE_FILE):
	for E in $(EXE_FILE) ; \
          do \
            echo "$(CXX) -g -o2 -o $$E $$E.cpp $(C_FLAGS) $(INC) $(LIB)"; \
            $(CXX) -g -o2 -o $$E $$E.cpp $(C_FLAGS) $(INC) $(LIB);\
          done 
#	for E in $(EXE_FILE) ; \
        do \
          echo "strip ./$$E ";\
          strip ./$$E ;\
        done
all:$(EXE_FILE) 
	@echo "$(EXE_FILE)"
clean:
	rm -f $(EXE_FILE)
