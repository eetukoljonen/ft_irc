/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 13:34:20 by ekoljone          #+#    #+#             */
/*   Updated: 2024/02/06 15:42:52 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
#define __SERVER_HPP__

# define MAX_CLIENTS 32

#include <sys/types.h>
#include <sys/socket.h>
#include "Server.hpp"
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
#include "Utils.hpp"

class Server
{
public:
	Server();
	~Server();
	// Server(int port, std::string pw, std::string name = "Garbaggio");
	void									startServer(std::string const &port, std::string const &pw);
	// getters
	std::string const						&getName() const;
	std::string const						&getPass() const;
	std::map<int, User *>					&getUsersMap();
	Channel									*getChannelByName(std::string const &name) const;
	User									*_findUserByNick(std::string const &nick) const;
	User									*_findUserByUsername(std::string const &username) const;
	std::vector<struct pollfd>::iterator	findPollStructByFd(int fd);
	// setters
	void									addNewChannel(Channel *channel);
	Channel									*createChannel(std::string const &name);
	void 									deleteUser(int fd);
	void									deleteChannel(Channel *channel);
private:
	std::string const						_name;
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

	Server(Server const &cpy);
	Server									&operator=(Server const &rhs);
};

#endif