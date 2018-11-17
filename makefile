#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
#

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: t2fs libs main


t2fs:
	$(CC) -c $(SRC_DIR)/t2fs.c -Wall -o $(BIN_DIR)/t2fs.o
	$(CC) -c $(SRC_DIR)/files.c -Wall -o $(BIN_DIR)/files.o
	$(CC) -c $(SRC_DIR)/disk.c -Wall -o $(BIN_DIR)/disk.o

libs:
	ar rcs $(LIB_DIR)/libt2fs.a $(BIN_DIR)/t2fs.o $(LIB_DIR)/apidisk.o $(BIN_DIR)/files.o $(BIN_DIR)/disk.o
main:
	gcc -c main.c
	gcc -o mp main.o $(BIN_DIR)/t2fs.o $(LIB_DIR)/apidisk.o $(BIN_DIR)/files.o $(BIN_DIR)/disk.o
	#lembrar de mudar o apidisk

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~
	rm -rf main.o mp


