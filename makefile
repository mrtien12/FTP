all:client.o server.o 
	g++ -pthread -c Server/request.cpp -o Server/request.o
	g++ -pthread -c Server/logic_handle.cpp -o Server/logic_handle.o
	g++ -pthread Client/client.o  -lsqlite3 -o Client/client
	g++ -pthread Server/server.o Server/request.o Server/logic_handle.o -lsqlite3 -o Server/server

client.o:
	g++ -c Client/client.c  -lsqlite3 -o Client/client.o
server.o:
	g++ -pthread -c Server/server.cpp -lsqlite3 -o Server/server.o


runs:
	@$(MAKE) -C Server runs

runc:
	./Client/client 127.0.0.1 5501

clean:
	rm -f Server/*.o
	rm -r Client/client.o
	rm -r Server/server.o
	rm -r Client/client
	rm -r Server/server


