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
#include <poll.h>

class Server
{
	public:
		Server(int port, std::string pw, std::string name = "Garbaggio");
		~Server(){};

	private:
		std::string _name;
		std::string _pw;
		int 		_port;
        int 		_sockfd;
		
		struct sockaddr_in _serverAddr;
		struct pollfd _pollfds[MAX_CLIENTS];
		
		void _bindSocket();
		void _createSocket();
		void _createPoll();
};

#endif