#include "Server.hpp"

void Server::_runServer()
{    	
	int numFds = _pollfds.size();
	int numEvents = poll(_pollfds, num_fds, 0);

    for (int i = 0; i < numEvents; ++i) 
	{
		if (i == 0 && _pollfds[i].fd & POLLIN) 
		{
			// Handle new connection  
		} 
		else 
		{
			// Handle I/O on existing connection
		}
    }
    // Perform any other periodic server tasks
}

Server::Server(int port, std::string pw, std::string name)
:_name(name), _pw(pw), _port(port), _sockfd(0)
{
	try 
	{
		_createSocket();
		_bindSocket();
		_createPoll();
	}
	catch (std::exception &e) {}
	
	while (1)
		_runServer();
}

void Server::_bindSocket()
{
	// init struct sockaddr_in
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(_port);
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(&(_serverAddr.sin_zero), 0, sizeof(_serverAddr));
    if (bind(_sockfd, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) == -1)
        throw std::exception();
    if (listen(_sockfd, MAX_CLIENTS) == -1)  // mark the socket as listening and set a max connections (backlog)
        throw std::exception();
}

void Server::_createSocket()
{
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (_sockfd == -1)
        throw std::exception();
	 if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) == -1)
        throw std::exception();
}

void Server::_createPoll()
{
	// init struct pollfd
    memset(_pollfds, 0, sizeof(_pollfds));
   	_pollfds[0].fd = _sockfd;
	_pollfds[0].events = POLLIN;
}