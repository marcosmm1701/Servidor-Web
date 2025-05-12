CC = gcc
CFLAGS = -g -Wall -pedantic

# Directorio donde se almacenarán los archivos .o
OBJ_DIR = obj

############################	exe 	############################

all: server client

server: $(OBJ_DIR)/connections.o $(OBJ_DIR)/parse.o $(OBJ_DIR)/response.o $(OBJ_DIR)/scripts.o $(OBJ_DIR)/config.o $(OBJ_DIR)/server.o 
	$(CC) $(CFLAGS) -o server $(OBJ_DIR)/server.o $(OBJ_DIR)/connections.o $(OBJ_DIR)/parse.o $(OBJ_DIR)/scripts.o $(OBJ_DIR)/response.o $(OBJ_DIR)/config.o

client: $(OBJ_DIR)/client.o $(OBJ_DIR)/connections.o $(OBJ_DIR)/client_utils.o $(OBJ_DIR)/scripts.o
	$(CC) $(CFLAGS) -o client $(OBJ_DIR)/client.o $(OBJ_DIR)/connections.o $(OBJ_DIR)/client_utils.o $(OBJ_DIR)/scripts.o


#########################	.o  	################################

# Crear el directorio obj si no existe
$(OBJ_DIR)/%.o: src/%.c includes/%.h
	@mkdir -p $(OBJ_DIR)  # Asegurarse de que el directorio obj exista
	$(CC) $(CFLAGS) -c $< -o $@

#########################	run 	###############################

run_server:
	./server

run_client:
	./client

runv_server:
	valgrind ./server 

runv_client:
	valgrind ./client 

########################clean##############################

clean:
	rm -rf $(OBJ_DIR) server client
