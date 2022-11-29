#=======================================================================
#
# Makefile
# --------
# Simple Makefile for building the reference model.
#
#=======================================================================

SRC = gimli_reference.c

CC = clang
CC_FLAGS = -Wall


gimli:	$(SRC)
	$(CC) $(CC_FLAGS) -o $@ $(SRC)


clean:
	rm -f ./gimli
	rm -f *.log
	rm -f *.txt
