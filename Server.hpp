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
#include "Channel.hpp"
#include <time.h>


class Server
{
public:
	Server(int port, std::string pw, std::string name = "Garbaggio");
	~Server(){};
	std::string const						&getName() const;
	std::string const						&getPass() const;
	std::map<int, User *>					&getUsersMap();
	Channel									*getChannelByName(std::string const &name) const;
	User									*_findUserByNick(std::string nick) const;
	void									addNewChannel(Channel *channel);
	std::vector<struct pollfd>::iterator	findPollStructByFd(int fd);
	void 									deleteUser(int fd);
	Channel									*createChannel(std::string const &name);
	// Channel								*createChannel(std::string const &name, std::string const &key);
private:
	std::string 							_name;
	std::string								_host;
	std::string 							_pw;
	int 									_port;
	int 									_listeningSocket;
	struct sockaddr_in						_serverAddr;
	std::vector<struct pollfd>				_pollfds;
	std::map<int, User *>					_usersMap;
	std::map<std::string, Channel *>		_channelMap;
	t_client								_client;
	time_t									_pingIntervalTimer;
	unsigned int							_pingMSG;								

	void									_runServer();
	void									_executeCommands(User *user);
	void									_bindSocket();
	void									_createSocket();
	void									_addPollFd(int fd);
	void									_acceptClient();
	void									_receiveMessage(int index);
	User									*_getUserByFd(const int fd);
	void									_broadcastServer(std::string const &msg);
	void									_pingUsers();
	void									_sendPingToUsers();
};

#endif