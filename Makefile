CC=g++
FLAGS=-Wall -std=c++17
FILES=$(wildcard *.cpp)
OUTPUT=gal
DOT=g.dot

build:
	$(CC) $(FILES) $(FLAGS) -o $(OUTPUT)

dot:
	dot $(DOT) -Tpng -o $(DOT).png
