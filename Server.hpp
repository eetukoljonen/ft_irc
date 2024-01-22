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
#include <map>
#include <string.h>
#include <arpa/inet.h>
#include <fstream>
#include "User.hpp"
#include "CommandExecution.hpp"
#include "Replies.hpp"


class Server
{
public:
	Server(int port, std::string pw, std::string name = "Garbaggio");
	~Server(){};
	std::string const			&getName() const;
	std::string const			&getPass() const;
	std::map<int, User *> const	&getUsersMap() const;
	User						*_findUserByNick(std::string nick) const;

private:
	std::string 				_name;
	std::string					_host;
	std::string 				_pw;
	int 						_port;
	int 						_listeningSocket;
	struct sockaddr_in			_serverAddr;
	std::vector<struct pollfd>	_pollfds;
	std::map<int, User *>		_usersMap;
	t_client					_client;
	
	void 	_clientRegistration(User &user);
	void	_runServer();
	void	_executeCommands(User *user);
	void	_bindSocket();
	void	_createSocket();
	void	_addPollFd(int fd);
	void	_acceptClient();
	void	_receiveMessage(int index);
	User	*_getUserByFd(const int fd);

};

#endif