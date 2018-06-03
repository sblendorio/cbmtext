UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
  STATIC := 
else
  STATIC := -static
endif

CXX=g++
CXXFLAGS=-std=c++11 -I.

cbmtext: cbmtext.o
	$(CXX) $(STATIC) -o cbmtext cbmtext.o 
