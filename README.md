# FTP
**Problem Statement:-** The aim of this project is to implement FTP application between a server and a client. The application is a console based application and is implemented using Unix Sockets. The project is developed in C++ language.

## Following FTP Commands are implemented:-

**1.	USER**  This command sends the user identification to the server.

**2.	PASS**  This command sends the user password to the server.

**3.  MKD**   This command causes the directory specified in the path name to be created as a directory.

**4.	CWD**   This command allows the user to change the current working directory to the path specified with the command.

**5.	RMD**   This command causes the directory specified in the path-name to be removed as a directory.

**6.	PWD**   This command causes the name of the current working directory to be returned in the reply.

**7.	RETR**  This command send the requested file over the data connection.

**8.	STOR**  This command causes to store a file into the current directory of the remote host.

**9.	LIST**  ends a request to display the list of all the files present in the directory.

**10.	ABOR**  This command tells the server to abort the previous FTP service command and any associated transfer of data.

**11.	QUIT**  This command terminates a USER and if file transfer is not in progress, the server closes the connection.

## Some of the FTP replies are:-
**1.	200**	  Command okay.

**2.	530** 	Not logged in.

**3.	331** 	User name okay, need password.

**4.	225** 	Data connection open; no transfer in progress.

**5.	502** 	Command not implemented.

**6.	503** 	Bad sequence of commands.
