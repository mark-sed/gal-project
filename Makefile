# Makefile for gal-project
# This constains 2 targets:
#    build  Which builds the application
#    debug  Which builds the application for development and debugging containing information logs
#    dot    Which creates a png image from passed in graph (in DOT variable)
#
# Use: make
#      make debug
#      make dot DOT=gal_output.dot
#
# Author: Marek Sedlacek
#

CC=g++
FLAGS=-Wall -std=c++17
FILES=$(wildcard *.cpp)
OUTPUT=gal
DOT=graph.dot

build:
	$(CC) $(FILES) $(FLAGS) -o $(OUTPUT).out

debug: 
	$(CC) $(FILES) $(FLAGS) -g -DDEBUG -o $(OUTPUT).out

dot:
	dot $(DOT) -Tpng -o $(DOT).png

install:
	python -m pip install -r requirements.txt
