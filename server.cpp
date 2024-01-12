#include "Server.hpp"

void Server::_runServer()
{    	
	nfds_t numFds = static_cast<nfds_t>(_pollfds.size());
	int numEvents = poll(&(_pollfds[0]), numFds, 0);
	if (numEvents == -1)
	{
		perror("poll");
		exit(1);
	}
    for (size_t i = 0; i < numFds; ++i) 
	{
		if (_pollfds[i].revents & (POLLIN | POLLOUT | POLLNVAL | POLLERR | POLLHUP))
		{
			if (_pollfds[i].fd == _listeningSocket) 
			{
				_client.fd = accept(_pollfds[0].fd, (struct sockaddr *)&_client.addr, &_client.addrLen);
				fcntl(_client.fd, F_SETFL, O_NONBLOCK);
				if (_client.fd == -1)
					perror("accept");
				else if (_client.fd)
				{
					_addPollFd(_client.fd);
					std::cout << "new connection" << std::endl;
				}
			}./
			else if (_pollfds[i].revents & (POLLIN))
			{
				char buf[512];
				memset(buf, '\0', 512);
				int nbytes = recv(_pollfds[i].fd, buf, 512, MSG_DONTWAIT);
				if (nbytes == 0)
				{
					std::cout << _pollfds[i].fd << " disconnected" << std::endl;
					close(_pollfds[i].fd);
					_pollfds.erase(_pollfds.begin() + i);
				}
				else if (nbytes == -1)
				{
					perror("recv");
				}
				else
				{
					std::cout << std::string(buf, 0, nbytes) << std::endl;
				}
			}
			if (_pollfds[i].revents & POLLIN)
					std::cout << "pollin" << std::endl;
			if (_pollfds[i].revents & POLLOUT)
					std::cout << "pollout" << std::endl << std::endl;
			//send
			//error (POLLNVAL, POLLERR, POLLHUP)
		}
    }
}

Server::Server(int port, std::string pw, std::string name)
:_name(name), _pw(pw), _port(port), _listeningSocket(0)
{
	try 
	{
		_createSocket();
		_bindSocket();
		_addPollFd(_listeningSocket);
	}
	catch (std::exception &e) {}
	
	while (1)
		_runServer();
	for (size_t i = 0; i < _pollfds.size(); i++)
		close(_pollfds[i].fd);
}

void Server::_bindSocket()
{
	// init struct sockaddr_in
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(_port);
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    // memset(&(_serverAddr.sin_zero), 0, sizeof(_serverAddr));
    if (bind(_listeningSocket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) == -1)
        throw std::exception();
    if (listen(_listeningSocket, MAX_CLIENTS) == -1)  // mark the socket as listening and set a max connections (backlog)
        throw std::exception();
}

void Server::_createSocket()
{
	_listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listeningSocket == -1)
        throw std::exception();
	if (fcntl(_listeningSocket, F_SETFL, O_NONBLOCK) == -1)
        throw std::exception();
}

void Server::_addPollFd(int fd)
{
	struct pollfd pollfd;
   	pollfd.fd = fd;
	pollfd.events = POLLIN | POLLOUT;
	pollfd.revents = 0;
	_pollfds.push_back(pollfd);
}