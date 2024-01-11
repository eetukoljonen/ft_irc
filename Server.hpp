#ifndef SERVER_HPP
# define SERVER_HPP

# define MAX_CLIENTS 30

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>

class Server
{
	public:
		Server(int port, std::string pw, std::string name = "Garbaggio");
		~Server(){};

	private:
		std::string _name;
		std::string _pw;
		int 		_port;
		int 		_pollfd;
        int 		_sockfd;
		struct sockaddr_in _serverAddr;
		
		void _bindSocket();
		void _createSocket();
};

#endif