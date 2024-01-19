/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atuliara <atuliara@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 14:59:36 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/19 11:47:09 by atuliara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool loop = true;

static void	signal_handler(int signal)
{
	(void)signal;
	loop = false;
}

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
		std::cout << ">>" << std::string(buf, 0, nbytes) << std::endl;
		//std::cout << "fd = " << _usersMap.find(_pollfds[index].fd)->second->getUserInfo().fd << std::endl;
		_usersMap.find(_pollfds[index].fd)->second->addToInputBuffer(std::string(buf, 0, nbytes));
	}
}

void Server::_acceptClient()
{
	User *newUser = new User;
	newUser->getUserInfo().fd = accept(_listeningSocket,
									(struct sockaddr *)&newUser->getUserInfo().addr,
									&newUser->getUserInfo().addrLen);
	fcntl(newUser->getUserInfo().fd, F_SETFL, O_NONBLOCK);
	if (newUser->getUserInfo().fd == -1)
	{
		delete newUser;
		perror("accept");
	}
	else if (newUser->getUserInfo().fd)
	{
		_addPollFd(newUser->getUserInfo().fd);
		std::cout << "new connection: " << std::string(inet_ntoa(newUser->getUserInfo().addr.sin_addr), 0, INET_ADDRSTRLEN + 1) << std::endl;
		_usersMap[newUser->getUserInfo().fd] = newUser;
	}
}

void	Server::_clientRegistration(User &user)
{
	user.addToSendBuffer(RPL_WELCOME(user_id(user.getNick(), user.getUser()), user.getNick()));
	//handle rest rpl later
	
	std::ifstream		data;
	char				filepath[5] = "motd";

	data.open(filepath);
	if (!data)
	{
		user.addToSendBuffer(ERR_NOMOTD(user.getNick()));
		return ;
	}
	else
	{
		std::string		motd_lines;
		std::string		buf;
		
		buf = RPL_MOTDSTART(user.getNick(), "ft_irc (localhost)");
		while (getline(data, motd_lines))
		{
			buf += RPL_MOTD(user.getNick(), motd_lines);
		}
		buf += RPL_ENDOFMOTD(user.getNick());
		user.addToSendBuffer(buf);
	}
	data.close();
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
			{
				_receiveMessage(i);
				//handle client registration
				_clientRegistration(*_getUserByFd(_pollfds[i].fd));
				CommandExecution::execute(*_getUserByFd(_pollfds[i].fd), *this);
			}
		}
		else if (_pollfds[i].revents & POLLOUT)
		{
			std::string msg = _getUserByFd(_pollfds[i].fd)->extractFromSendBuffer();
			// std::cout << "msg to client: " << msg << std::endl;
			if (!msg.empty())
				send(_pollfds[i].fd, msg.c_str(), msg.size(), 0);
		}
		else if (_pollfds[i].revents & (POLLNVAL | POLLERR | POLLHUP))
		{
			std::cout << "POLLERR STUFF" << std::endl;
			exit (1);
		}
    }
}

User* Server::_getUserByFd(const int fd)
{
   	auto it = _usersMap.find(fd);
    if (it != _usersMap.end())
	{
		//std::cout << user->getUserInfo().fd << std::endl;
		return it->second;
	}
	else
	{
		std::cout << "couldnt find fd from map" << std::endl;
		return nullptr;
	}  
}

Server::Server(int port, std::string pw, std::string name)
:_name(name), _pw(pw), _port(port), _listeningSocket(0)
{
	// std::string test1 = ":Nick!user@hostname PRIVMSG #example_channel :Hello, this is a regular message in a channel!\r\n";

	// User testman;
	// testman.appendInput(test1);
	try 
	{
		_createSocket();
		_bindSocket();
		_addPollFd(_listeningSocket);
	}
	catch (std::exception &e) {}
	
	signal(SIGINT, signal_handler);
	while (loop)
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
	_host = inet_ntoa( _serverAddr.sin_addr);
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