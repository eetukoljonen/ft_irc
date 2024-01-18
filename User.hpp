/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 14:39:53 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/18 17:03:05 by ekoljone         ###   ########.fr       */
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
	int							addToInputBuffer(std::string input);
	std::string const			extractInput();
	t_client 					&getUserInfo();
	void						addToSendBuffer(std::string const &msg);
	std::string const			extractFromSendBuffer();
private:
	std::string					_nick;
	std::string					_user;
	std::string					_ipAddr;
	t_client					_userInfo;
	std::vector<std::string>	_userInput;
	std::vector<std::string>	_sendBuffer;
	// bool						_isRegistered;
};

#endif
