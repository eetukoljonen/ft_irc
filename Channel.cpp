/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 10:53:28 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/19 11:43:39 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(){}

Channel::Channel(Channel const &cpy)
{
	_users = cpy._users;
	_bannedUsers = cpy._bannedUsers;
	_kickedUsers = cpy._kickedUsers;
	_operators = cpy._operators;
}

Channel::~Channel(){}

Channel &Channel::operator=(Channel const &rhs)
{
    if (this != &rhs)
    {
		_users = rhs._users;
		_bannedUsers = rhs._bannedUsers;
		_kickedUsers = rhs._kickedUsers;
		_operators = rhs._operators;
    }
    return (*this);
}

bool Channel::isBanned(std::string const &nick)
{
	if (std::find(_bannedUsers.begin(), _bannedUsers.end(), nick) != _bannedUsers.end())
		return (true);
	return (false);
}

bool Channel::isKicked(std::string const &nick)
{
	if (std::find(_kickedUsers.begin(), _kickedUsers.end(), nick) != _kickedUsers.end())
		return (true);
	return (false);
}

bool Channel::isOperator(std::string const &nick)
{
	if (std::find(_operators.begin(), _operators.end(), nick) != _operators.end())
		return (true);
	return (false);
}

