/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/02/12 12:04:07 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/Server.hpp"

Server::Server() : _name("SocSyncServ"), _port(0), _listeningSocket(0), _pingIntervalTimer(time(0)), _pingMSG(0) {}

Server::Server(Server const &cpy)
{
	(void)cpy;
}

Server &Server::operator=(Server const &rhs)
{
	(void)rhs;
	return (*this);
}


Server::~Server()
{
	std::map<int, User *>::iterator userIt					=	_usersMap.begin();
	std::map<int, User *>::iterator userIte					=	_usersMap.end();
	while (userIt != userIte)
	{
		if (userIt->second)
			delete userIt->second;
		userIt++;
	}
	std::map<std::string, Channel *>::iterator channelIt	=	_channelMap.begin();
	std::map<std::string, Channel *>::iterator channelIte	=	_channelMap.end();
	while (channelIt != channelIte)
	{
		if (channelIt->second)
			delete channelIt->second;
		channelIt++;
	}
	for (size_t i = 0; i < _pollfds.size(); i++)
		close(_pollfds[i].fd);
}

bool loop = true;

static void	signal_handler(int signal)
{
	if (signal == SIGINT)
	{
		std::cout << PURPLE << "SocSyncServ closed. Thank you, come again!" << RESET << std::endl;
		loop = false;
	}
}

void Server::_printStart()
{
	std::cout << GREEN	<<  "  ________________  _________            \n"
							" /   _____\\_____  \\ \\_   ___ \\           \n"
							" \\_____  \\ /   |   \\/    \\  \\/           \n"
							" /        /    |    \\     \\____          \n"
							" /_______  \\_______  /\\______  /          \n"
							"          \\/        \\/        \\/           \n"
							" _____________.___._______  _________   \n"
							"/   _____\\__  |   |\\      \\ \\_   ___ \\  \n"
							"\\_____  \\ /   |   |/   |   \\/    \\  \\/  \n"
							"/        \\\\____   /    |    \\     \\____ \n"
							"/_______  // ______\\____|__  /\\______  / \n"
							"         \\/ \\/              \\/        \\/  \n"
							"________________________________   ____\n"
							"/   _____\\_   _____\\______   \\   \\ /   /\n"
							"\\_____  \\ |    __)_ |       _/\\   Y   / \n"
							"/        \\|        \\|    |   \\ \\     /  \n"
							"/_______  /_______  /|____|_  /  \\___/   \n"
							"        \\/        \\/        \\/           \n" << RESET;
	std::cout	<< YELLOW << "is starting..." << RESET << std::endl
				<< "Connect with Irssi:		/connect localhost " << _port << " " << _pw << std::endl
				<< "Connect with Netcat		nc 127.0.0.1 " << _port << " " << _pw << std::endl;
}

void Server::startServer(std::string const &port, std::string const &pw)
{
	if (pw.size() < 1 || pw.size() > 10)
	{
		std::cerr << RED << "invalid password, password has to be between 1 - 10 characters" << RESET << std::endl;
		return ;
	}
	_pw = pw;
	try{_port = std::stoi(port);}
	catch(const std::exception& e){std::cerr << RED << "invalid port" << RESET << std::endl; return ;}
	if (_port < 1 || _port > 65535)
	{
		std::cerr << RED << "invalid port" << RESET << std::endl;
		return ;
	}
	_printStart();
	signal(SIGINT, signal_handler);
	try 
	{
		_createSocket();
		_bindSocket();
		_addPollFd(_listeningSocket);
		_runServer();
	}
	catch (std::exception &e){}
}

void Server::_receiveMessage(int index, User *currentUser)
{
	char buf[510];
	memset(buf, '\0', 510);
	int nbytes = recv(_pollfds[index].fd, buf, 510, MSG_DONTWAIT);
	if (nbytes == 0)
	{
		if (currentUser && currentUser->isRegistered())
			std::cout << PURPLE << user_id(currentUser->getNick(), currentUser->getUser(), currentUser->getIP()) << " disconnected" << RESET << std::endl;
		_connectionError(_pollfds[index].fd, currentUser);
	}
	else if (nbytes == -1 && errno == 54) 
		perror("recv");
	else if (nbytes == -1)
	{
		perror("recv");
		throw std::exception();
	}
	else
		currentUser->addToInputBuffer(std::string(buf, 0, nbytes));
}

void Server::_acceptClient()
{
	t_client client_info;
	client_info.fd = accept(_listeningSocket,
							(struct sockaddr *)&client_info.addr,
							&client_info.addrLen);
	if (client_info.fd == -1 || fcntl(client_info.fd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("accept");
		throw std::exception();
	}
	else if (client_info.fd)
	{
		if (_usersMap.size() >= MAX_CLIENTS)
		{
			std::string const errormsg = "ERROR: Server is full. Please try again later.\r\n";
			// send(client_info.fd, errormsg.c_str(), 49, 0);
			// std:: cout << RED <<  "<< " << errormsg << RESET;
			_killUser(client_info.fd, nullptr, errormsg);
			return ;
		}
		User *newUser = new User;
		newUser->setClientInfo(client_info);
		_addPollFd(client_info.fd);
		std::cout << PURPLE << "New connection!" << RESET << std::endl;
		_usersMap[client_info.fd] = newUser;
		newUser->setIP(inet_ntoa(client_info.addr.sin_addr));
	}
}

void Server::_executeCommands(User *user)
{
	while (1)
	{
		std::string input = user->extractInput();
		if (input.empty())
			break ;
		std::cout << YELLOW << ">> " << input << RESET;
		if (!user->isRestricted())
		{
			Command cmd(input);
			CommandExecution::execute(user, this, cmd);
		}
		else
			user->addToSendBuffer(ERR_RESTRICTED(_name));
	}
}

void Server::_killUser(int fd, User *currentUser, std::string const &reason)
{
	send(fd, reason.c_str(), reason.size(), 0);
	std::cout << RED <<  "<< " << reason << RESET;
	_connectionError(fd, currentUser);
}

void Server::_sendMessage(int fd, User *currentUser)
{
	if (fd != _listeningSocket)
	{
		std::string msg;
		while (1)
		{
			std::string tmp = currentUser->extractFromSendBuffer();
			if (tmp.empty())
				break ;
			msg.append(tmp);
		}
		if (!msg.empty())
		{
			int bytes = send(fd, msg.c_str(), msg.size(), 0);
			std::cout << GREEN << "<< " << msg.substr(0, bytes) << RESET;
			if (bytes == -1)
			{
				perror("send");
				throw std::exception();
			}
			if (static_cast<size_t>(bytes) < msg.size())
			{
				msg = msg.substr(0, bytes);
				currentUser->addToInputBufferFront(msg);
			}
		}
		if (currentUser->isRestricted())
		{
			_killUser(fd, currentUser, ERROR(currentUser->getNick(), _name, "Connection restricted"));
			return ;
		}
	}
}

void Server::_connectionError(int fd, User *currentUser)
{
	if (currentUser)
	{
		std::vector<Channel *> userChannels = currentUser->getChannels();
		if (!userChannels.empty())
		{
			std::vector<Channel *>::iterator it = userChannels.begin();
			std::vector<Channel *>::iterator ite = userChannels.end();
			while (it != ite)
			{
				Channel *channel = *it;
				channel->removeFromChannel(currentUser->getNick());
				channel->broadcastToChannel(QUIT(user_id(currentUser->getNick(), currentUser->getUser(), currentUser->getIP()), ":connection lost"));
				it++;
				if (channel->getUserCount() <= 0)
					deleteChannel(channel);
			}
		}
	}
	deleteUser(fd);
}

void Server::_runServer()
{
	while (loop)
	{
		nfds_t numFds = static_cast<nfds_t>(_pollfds.size());
		if (poll(&(_pollfds[0]), numFds, 0) == -1)
		{
			perror("poll");
			throw std::exception();
		}
		size_t i = 0;
		while (i < _pollfds.size())
		{
			User *currentUser = _getUserByFd(_pollfds[i].fd);
			if (_pollfds[i].revents & POLLIN)
			{
				if (_pollfds[i].fd == _listeningSocket) 
					_acceptClient();
				else
				{
					_receiveMessage(i, currentUser);
					_executeCommands(currentUser);
				}
			}
			else if (_pollfds[i].revents & POLLOUT)
				_sendMessage(_pollfds[i].fd, currentUser);
			else if (_pollfds[i].revents & (POLLNVAL | POLLERR | POLLHUP))
				_connectionError(_pollfds[i].fd, currentUser);
			// checking if ping interval timer has gone over 60 secs
			time_t currentTime = time(0);
			if (currentTime - _pingIntervalTimer >= 60)
				_pingUsers();
			i++;
		}
	}
}

User* Server::_getUserByFd(const int fd)
{
   	auto it = _usersMap.find(fd);
    if (it != _usersMap.end())
		return (it->second);
	return (nullptr);
}

void Server::_bindSocket()
{
	// init struct sockaddr_in
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(_port);
    _serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(_listeningSocket, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) == -1)
    {
		perror("bind");
		throw std::exception();
	}
    if (listen(_listeningSocket, MAX_CLIENTS) == -1)  // mark the socket as listening and set a max connections (backlog)
    {
		perror("listen");
		throw std::exception();
	}
	_host = inet_ntoa( _serverAddr.sin_addr);
}

void Server::_createSocket()
{
	_listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_listeningSocket == -1)
	{
		perror("socket");
		throw std::exception();
	}
	if (fcntl(_listeningSocket, F_SETFL, O_NONBLOCK) == -1)
    {
		perror("fcntl");
		throw std::exception();
	}
}

void Server::_addPollFd(int fd)
{
	struct pollfd pollfd;
   	pollfd.fd = fd;
	pollfd.events = POLLIN | POLLOUT;
	pollfd.revents = 0;
	_pollfds.push_back(pollfd);
}

std::string const &Server::getName() const {
	return _name;
}

std::string const &Server::getPass() const {
	return _pw;
}

std::map<int, User *> &Server::getUsersMap() {
	return _usersMap;
}

User *Server::_findUserByNick(std::string const &nick) const
{
	auto it = _usersMap.begin();
	while (it != _usersMap.end())
	{
		if (!it->second->getNick().compare(nick))
			return it->second;
		it++;
	}
	return (nullptr);
}

User *Server::_findUserByUsername(std::string const &username) const
{
	std::map<int, User *>::const_iterator it = _usersMap.cbegin();
	std::map<int, User *>::const_iterator ite = _usersMap.cend();
	while (it != ite)
	{
		if (!it->second->getUser().compare(username))
			return (it->second);
		it++;
	}
	return (nullptr);
}

void Server::addNewChannel(Channel *channel)
{
	_channelMap[channel->getChannelName()] = channel;
}

Channel *Server::getChannelByName(std::string const &name) const
{
	std::map<std::string, Channel *>::const_iterator it = _channelMap.cbegin();
	std::map<std::string, Channel *>::const_iterator ite = _channelMap.cend();
	while (it != ite)
	{
		// putting the channelname to upper because channel names are case insensitive
		std::string const &channelName = str_toupper(it->second->getChannelName());
		std::string const &parameter = str_toupper(name);
		if (!channelName.compare(parameter))
			return (it->second);
		it++;
	}
	return (nullptr);
}

std::vector<struct pollfd>::iterator Server::findPollStructByFd(int fd)
{
	auto it = _pollfds.begin();
	while (it != _pollfds.end())
	{
		if (it->fd == fd)	
			return it;
		it++;
	}
	return it;
}

void Server::deleteUser(int fd)
{
	auto it_map = _usersMap.find(fd);
	auto it_poll = findPollStructByFd(fd);
	if (it_map != _usersMap.end() || it_poll != _pollfds.end()) 
	{
		if (it_map != _usersMap.end())
		{
			if (it_map->second)
				delete it_map->second;
			_usersMap.erase(it_map);
		}
		if (it_poll != _pollfds.end())
			_pollfds.erase(it_poll);
	}
	close(fd);
}

Channel *Server::createChannel(std::string const &name)
{
	Channel *channel = new Channel;
	channel->setChannelName(name);
	addNewChannel(channel);
	return (channel);
}

void Server::_broadcastServer(std::string const &msg)
{
	std::map<int, User *>::iterator it = _usersMap.begin();
	std::map<int, User *>::iterator ite = _usersMap.end();
	while (it != ite)
	{
		it->second->addToSendBuffer(msg);
		it++;
	}
}

void Server::_sendPingToUsers()
{
	std::map<int, User *>::iterator it = _usersMap.begin();
	std::map<int, User *>::iterator ite = _usersMap.end();
	std::string ping_msg;
	while (it != ite)
	{
		//creating unique ping msg to users
		ping_msg = std::to_string(_pingMSG++);
		// saving it to user class so when user responds we expect the message to match
		it->second->setPongResponse(ping_msg);
		it->second->addToSendBuffer(PING(_name, ping_msg));
		it++;
	}
}

void Server::_pingUsers()
{
	// resetting the ping interval timer
	_pingIntervalTimer = time(0);
	if (_usersMap.empty())
		return ;
	// checking the ping response timers that they haven't
	// gone over the 1 minute limit
	time_t curTime = time(0);
	std::map<int, User *>::iterator it = _usersMap.begin();
	while (it != _usersMap.end())
	{
		time_t userTimer = it->second->getPingResponseTimer();
		if (curTime - userTimer >= 120)
		{
			User *user = it->second;
			std::string const &errorMsg = ERROR(user->getNick(), _name, "Ping timeout");
			++it;
			_killUser(user->getUserInfo().fd , user, errorMsg);
		}
		else
			it++;
	}
	_sendPingToUsers();
}

void Server::deleteChannel(Channel *channel)
{
	std::map<std::string, Channel *>::iterator it = _channelMap.find(channel->getChannelName());
	if (it != _channelMap.end())
	{
		if (it->second)
			delete it->second;
		_channelMap.erase(it);
	}
}