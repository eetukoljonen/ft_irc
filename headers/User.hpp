/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 14:39:53 by ekoljone          #+#    #+#             */
/*   Updated: 2024/02/09 12:43:30 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __USER_HPP__
#define __USER_HPP__

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
#include <sstream>
#include <iostream>
#include <vector>
#include <time.h>

class Channel;

typedef struct s_client
{
	int							fd;
	struct sockaddr_in			addr;
	socklen_t					addrLen = sizeof(addr);
}	t_client;

class User
{
public:
    User();
    User(User const &cpy);
    ~User();
    User						&operator=(User const &rhs);
	//bools
	bool const					&isRegistered() const;
	bool const					&isPassCorrect() const;
	bool const					&isRestricted() const;
	//getters
	std::string const			&getNick();
	std::string const			&getUser();
	time_t const				&getPingResponseTimer();
	std::string const			&getUserMode() const;
	std::string const			&getIP() const;
	t_client					&getUserInfo();
	std::string const			extractInput();
	std::vector<Channel *>		getChannels() const;
	std::string const			extractFromSendBuffer();
	std::string const			&getPongResponse() const;
	//setters
	void						addToInputBuffer(std::string input);
	void						addToInputBufferFront(std::string const &input);
	void						addToSendBuffer(std::string const &msg);
	void						setNick(std::string const &nick);
	void						setUser(std::string const &user);
	void						setRegistrationFlag(bool flag);
	void						setPassFlag(bool flag);
	void						removeUserMode(std::string const &mode);
	void						addUserMode(std::string const &mode);
	void						setIP(std::string const &ip);
	void						resetPingResponseTimer();
	void						setPongResponse(std::string const &msg);
	void						addNewChannel(Channel *channel);
	void						setClientInfo(t_client const &info);
	void						removeChannel(Channel *channel);
	void						restrictUser();
private:
	std::string					_nick;
	std::string					_user;
	std::string					_ipAddr;
	t_client					_userInfo;
	std::vector<std::string>	_userInput;
	std::vector<std::string>	_sendBuffer;
	std::vector<Channel *>		_channels;
	std::string					_userModes;
	bool						_isRegistered;
	bool						_passFlag;
	time_t						_pingResponseTimer;
	std::string					_pongRespone;
	bool						_isRestricted;
};

#endif
