CC = gcc  
SERVER = cubricd 
CLIENT = cubricc  
default = all  

all: $(SERVER) $(CLIENT)


$(SERVER): server/main.o server/daemon.o server/network.o   
		$(CC) -o bin/$@ server/main.o server/daemon.o server/network.o 
		
$(CLIENT): client/main.c 
		$(CC) -o bin/$@ client/main.c 
		
main.o: server/main.c server/defs.h
		$(CC) -o $@ -c server/main.c
		
daemon.o: server/daemon.c server/defs.h
		$(CC) -o $@ -c server/daemon.c 

network.o: server/network.c server/defs.h 
		$(CC) -o $@ -c server/network.c 
		
clean: 
	rm -rf bin/* server/*.o client/*.o shared/*.o 

