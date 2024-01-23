/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 10:53:28 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/23 17:13:30 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel() : _invateOnly(false), _userLimit(100) {}

Channel::Channel(Channel const &cpy)
{
	_users = cpy._users;
	_bannedUsers = cpy._bannedUsers;
	_kickedUsers = cpy._kickedUsers;
	_operators = cpy._operators;
	_invateOnly = cpy._invateOnly;
	_userLimit = cpy._userLimit;
	_topic = cpy._topic;
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
		_invateOnly = rhs._invateOnly;
		_userLimit = rhs._userLimit;
		_topic = rhs._topic;
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

void Channel::addToChannel(User *user)
{
	_users[user->getNick()] = user;
}

void Channel::addToOperators(std::string const &nick)
{
	_operators.push_back(nick);
}

void Channel::addToBanList(std::string const &nick)
{
	_bannedUsers.push_back(nick);
}

void Channel::addToKickList(std::string const &nick)
{
	_kickedUsers.push_back(nick);
}

bool Channel::isInvateOnly()
{
	return (_invateOnly);
}

void Channel::setChannelName(std::string const &name)
{
	_channelName = name;
}

std::string const &Channel::getChannelName() const
{
	return (_channelName);
}

void Channel::setChannelKey(std::string const &key)
{
	_channelKey = key;
}

std::string const &Channel::getChannelkey() const
{
	return (_channelKey);
}
