CC=g++
FLAGS=-Wall -std=c++17 -g
FILES=$(wildcard *.cpp)
OUTPUT=gal
DOT=g.dot

build:
	$(CC) $(FILES) $(FLAGS) -o $(OUTPUT).out

dot:
	dot $(DOT) -Tpng -o $(DOT).png
