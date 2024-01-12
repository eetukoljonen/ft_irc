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
#include <vector>
#include <string.h>

typedef struct s_client
{
	int					fd;
	struct sockaddr_in	addr;
	socklen_t			addrLen = sizeof(addr);
}	t_client;

class Server
{
	public:
		Server(int port, std::string pw, std::string name = "Garbaggio");
		~Server(){};

	private:
		std::string 				_name;
		std::string 				_pw;
		int 						_port;
        int 						_listeningSocket;
		struct sockaddr_in			_serverAddr;
		std::vector<struct pollfd>	_pollfds;
		t_client					_client;
		
		void _runServer();
		void _bindSocket();
		void _createSocket();
		void _addPollFd(int fd);
};

#endif