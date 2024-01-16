/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 14:59:36 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/16 16:43:16 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::_receiveMessage(int index)
{
	char buf[512];
	memset(buf, '\0', 512);
	int nbytes = recv(_pollfds[index].fd, buf, 512, MSG_DONTWAIT);
	if (nbytes == 0)
	{
		std::cout << _pollfds[index].fd << " disconnected" << std::endl;
		close(_pollfds[index].fd);
		_pollfds.erase(_pollfds.begin() + index);
	}
	else if (nbytes == -1)
	{
		perror("recv");
	}
	else
	{
		std::cout << std::string(buf, 0, nbytes) << std::endl;
		_usersMap.find(_pollfds[i].fd)->second.appendInput(std::string(buf, 0, nbytes));
	}
}

void Server::_acceptClient()
{
	User newUser;
	newUser.getUserInfo().fd = accept(_listeningSocket,
									(struct sockaddr *)&newUser.getUserInfo().addr,
									&newUser.getUserInfo().addrLen);
	fcntl(_client.fd, F_SETFL, O_NONBLOCK);
	if (newUser.getUserInfo().fd == -1)
		perror("accept");
	else if (newUser.getUserInfo().fd)
	{
		_addPollFd(newUser.getUserInfo().fd);
		std::cout << "new connection: " << std::string(inet_ntoa(newUser.getUserInfo().addr.sin_addr), 0, INET_ADDRSTRLEN + 1) << std::endl;
		_usersMap[newUser.getUserInfo().fd] = newUser;
	}
}

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
		if (_pollfds[i].revents & POLLIN)
		{
			if (_pollfds[i].fd == _listeningSocket) 
				_acceptClient();
			else
				_receiveMessage(i);
		}
		else if (_pollfds[i].revents & POLLOUT)
		{
			for (size_t j = 1; j < numFds && i > 0; j++)
			{
				if (j != i)
				{
					std::string msg(_usersMap.find(_pollfds[j].fd)->second.getMessage());
					std::cout << msg << std::endl;
					if (!msg.empty())
					{
						int nbytes = send(_pollfds[i].fd, msg.c_str(), msg.size() + 1, 0);
						if (nbytes == -1)
							perror("send");
					}
				}
			}
			// else if (_pollfds[i].revents & (POLLNVAL | POLLERR | POLLHUP))
			// {

			// }
			// if (_pollfds[i].revents & POLLIN)
			// 		std::cout << "pollin" << std::endl;
			// if (_pollfds[i].revents & POLLOUT)
			// 		std::cout << "pollout" << std::endl << std::endl;
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