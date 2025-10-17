# Author: David Holmqvist <daae19@student.bth.se>

CXX=g++-13
CXXFLAGS=-std=c++17 -pthread -g -Wunused -Wall -Wunused

all: pearson pearson_par verify

pearson: vector dataset analysis pearson.cpp 
	$(CXX) $(CXXFLAGS) pearson.cpp vector.o dataset.o analysis.o -o pearson

pearson_par: vector dataset analysis pearson.cpp 
	$(CXX) $(CXXFLAGS) pearson_par.cpp vector.o dataset.o analysis_par.o -o pearson_par

analysis: vector analysis.hpp analysis.cpp analysis_par.hpp analysis_par.cpp
	$(CXX) $(CXXFLAGS) -c analysis.cpp -o analysis.o
	$(CXX) $(CXXFLAGS) -c analysis_par.cpp -o analysis_par.o

dataset: vector dataset.hpp dataset.cpp
	$(CXX) $(CXXFLAGS) -c dataset.cpp -o dataset.o

vector: vector.hpp vector.cpp
	$(CXX) $(CXXFLAGS) -c vector.cpp -o vector.o

verify: verify.c
	$(CC) verify.c -o verify

clean:
	rm -rf verify pearson pearson_par *.o *.dSYM 2> /dev/null
