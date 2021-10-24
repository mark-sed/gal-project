# Project: Graph algorithms
# Author: Jiri Kristof
# Date: 23-10-2021

CC=g++
INPUT=main.cpp src/Graph.cpp
OUTPUT=proj
CFLAGS=-Wall -pedantic -I boost
FILENAME=graph.dot

.PHONY: run

compile:
	${CC} ${CFLAGS} ${INPUT} -o ${OUTPUT}

# run example:
# 	make run FILENAME=name_of_file.dot
run:
	./${OUTPUT} -f ${FILENAME}
