#include "Server.hpp"

Server::Server(int port, std::string pw, std::string name)
:_name(name), _pw(pw), _port(port), _pollfd(0), _sockfd(0)
{
	// socket creation
	try 
	{
		_createSocket();
		_bindSocket();
		    // Accepting a connection
		int new_socket;
		socklen_t socklen = sizeof(_serverAddr);
   		if ((new_socket = accept(_sockfd, (struct sockaddr *)&_serverAddr, &socklen)) < 0)
		{
        	perror("accept");
        	exit(EXIT_FAILURE);
    	}
		char *buffer = NULL;
    	// Reading message from client
    	read(new_socket, buffer, 1024);
    	std::cout << "Message from client: " << buffer << std::endl;

    	// Closing the connection
    	close(new_socket);
    	close(_sockfd);
		(void)_pollfd;
	}
	catch (std::exception &e) {}
}

void    Server::_bindSocket()
{
    _serverAddr.sin_family = AF_INET; // host byte order
    _serverAddr.sin_port = htons(_port);
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(_serverAddr.sin_zero), 0, 8); // fill remaining space with 0
    if (bind(_sockfd, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) == -1)
        throw std::exception();
    // mark the socket as listening and set a max connections (backlog)
    if (listen(_sockfd, MAX_CLIENTS) == -1)
        throw std::exception();
}

void Server::_createSocket()
{
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == -1)
        throw std::exception();
	//  if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) == -1)
    //     throw std::exception();
}