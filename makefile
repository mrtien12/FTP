all:client.o server.o
	g++  Client/client.o  -lsqlite3 -o Client/client
	g++  Server/server.o  -lsqlite3 -o Server/server
	g++ Server/database.o  -lsqlite3 -o Server/database
client.o:
	g++ -c Client/client.cpp  -lsqlite3 -o Client/client.o
server.o:
	g++ -c Server/server.cpp -lsqlite3 -o Server/server.o

database.o:
	g++ -c Server/database.cpp -lsqlite3 -o Server/database.o


clean:
	rm -r Client/client.o
	rm -r Server/server.o
	rm -r Client/client
	rm -r Server/server
	rm -r Server/database.o
	rm -r Server/database
	

