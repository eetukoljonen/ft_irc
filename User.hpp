/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 14:39:53 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/17 16:08:31 by ekoljone         ###   ########.fr       */
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


typedef struct s_command
{
	std::string					prefix;
	std::string					command;
	std::vector<std::string>	params;
}	t_command;

typedef struct s_client
{
	int					fd;
	struct sockaddr_in	addr;
	socklen_t			addrLen = sizeof(addr);
}	t_client;

class User
{
public:
    User();
    User(User const &cpy);
    ~User();
    User &operator=(User const &rhs);
	t_command					*extractCommand();
	int							appendInput(std::string input);
	std::string const			extractInput();
	t_client 					&getUserInfo();
private:
	std::string					_name;
	std::string					_ipAddr;
	t_client					_userInfo;
	std::vector<std::string>	_userInput;
};

#endif
