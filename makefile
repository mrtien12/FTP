all:client.o server.o database.o
	g++ -pthread -c Server/request.cpp -o Server/request.o
	g++ -pthread -c Server/logic_handle.cpp -o Server/logic_handle.o
	g++ -pthread Client/client.o  -lsqlite3 -o Client/client
	g++ -pthread Server/server.o Server/request.o Server/logic_handle.o -lsqlite3 -o Server/server
	g++ -pthread Server/database.o  -lsqlite3 -o Server/database
client.o:
	g++ -c Client/client.c  -lsqlite3 -o Client/client.o
server.o:
	g++ -pthread -c Server/server.cpp -lsqlite3 -o Server/server.o

database.o:
	g++ -pthread -c Server/database.cpp -lsqlite3 -o Server/database.o

runs:
	./Server/server 5500

runc:
	./Client/client 127.0.0.1 5500

clean:
	rm -f Server/*.o
	rm -r Client/client.o
	rm -r Server/server.o
	rm -r Client/client
	rm -r Server/server
	rm -r Server/database.o
	rm -r Server/database
	

