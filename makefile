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
TEST_DIR=./exemplo
all: t2fs libs 


t2fs:
	$(CC)  -Wall -c $(SRC_DIR)/files.c -o $(BIN_DIR)/files.o  -I$(INC_DIR)
	$(CC)  -Wall -c $(SRC_DIR)/disk.c  -o $(BIN_DIR)/disk.o   -I$(INC_DIR)
	$(CC)  -Wall -c $(SRC_DIR)/fat.c   -o $(BIN_DIR)/fat.o    -I$(INC_DIR)
	$(CC)  -Wall -c $(SRC_DIR)/util.c  -o $(BIN_DIR)/util.o   -I$(INC_DIR)
	$(CC)  -Wall -c $(SRC_DIR)/t2fs.c  -o $(BIN_DIR)/t2fs.o   -I$(INC_DIR)

libs:
	ar crs $(LIB_DIR)/libt2fs.a  $(LIB_DIR)/apidisk.o $(BIN_DIR)/*.o
main:
	cd TEST_DIR
	gcc -c main.c
	gcc -o teste main.o $(BIN_DIR)/t2fs.o $(LIB_DIR)/apidisk.o $(BIN_DIR)/files.o $(BIN_DIR)/disk.o $(BIN_DIR)/fat.o $(BIN_DIR)/util.o
	cd ..

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~



