.SUFFIXES:
TARGETS=bfs graph

CC=gcc
LD=gcc

CXX=g++
LD=g++

CFLAGS=-g -Wfatal-errors -Ofast -fomit-frame-pointer
CXXFLAGS=-g -Wfatal-errors -Ofast -fomit-frame-pointer --std=c++1z

LFLAGS=-g
HEADERS:=$(wildcard *.h) Makefile

all: $(TARGETS)

%.o: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

%: %.cc $(HEADERS)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf *.o *.dSYM $(TARGETS)
