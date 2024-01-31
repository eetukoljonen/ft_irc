/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 10:53:28 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/31 10:43:59 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel() : _userLimit(100), _modes(0) {}

Channel::Channel(Channel const &cpy)
{
	_users = cpy._users;
	_invitedUsers = cpy._invitedUsers;
	_kickedUsers = cpy._kickedUsers;
	_operators = cpy._operators;
	_userLimit = cpy._userLimit;
	_topic = cpy._topic;
	_modes = cpy._modes;
}

Channel::~Channel(){}

Channel &Channel::operator=(Channel const &rhs)
{
    if (this != &rhs)
    {
		_users = rhs._users;
		_invitedUsers = rhs._invitedUsers;
		_kickedUsers = rhs._kickedUsers;
		_operators = rhs._operators;
		_userLimit = rhs._userLimit;
		_topic = rhs._topic;
		_modes = rhs._modes;
    }
    return (*this);
}

bool Channel::isInvited(std::string const &nick)
{
	if (std::find(_invitedUsers.begin(), _invitedUsers.end(), nick) != _invitedUsers.end())
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
	std::string const &nick = user->getNick();
	_users[nick] = user;
	if (!_nickList.empty())
		_nickList.append(" ");
	_nickList.append(nick);
}

void Channel::addToOperators(std::string const &nick)
{
	_operators.push_back(nick);
}

void Channel::addToInviteList(std::string const &nick)
{
	_invitedUsers.push_back(nick);
}

void Channel::addToKickList(std::string const &nick)
{
	_kickedUsers.push_back(nick);
}

void Channel::setChannelName(std::string const &name)
{
	_channelName = name;
}

std::string const &Channel::getChannelName() const
{
	return _channelName;
}

void Channel::setChannelKey(std::string const &key)
{
	_channelKey = key;
}

std::string const &Channel::getChannelkey() const
{
	return (_channelKey);
}

std::string const &Channel::getNickList() const
{
	return (_nickList);
}

std::string const &Channel::getTopic() const
{
	return (_topic);
}

int const &Channel::getUserLimit() const
{
	return (_userLimit);
}

int Channel::getUserCount() const
{
	return (_users.size());
}

bool Channel::UserOnChannel(std::string const &nick)
{
	auto it = _users.find(nick);
	if (it != _users.end())
		return (true);
	return (false);
}

User	*Channel::getUser(std::string const &nick) const
{
	std::map<std::string, User *>::const_iterator it = _users.find(nick);
	if (it != _users.end())
		return it->second;
	return (nullptr);
}

std::map<std::string, User *>	&Channel::getUsersMap()
{
	return _users;
}

void Channel::broadcastToChannel(std::string const & msg) 
{
	std::map<std::string, User *>::iterator it;
	
    for(it = _users.begin(); it != _users.end(); ++it)
	{
        it->second->addToSendBuffer(msg);
	}
}

void	Channel::removeFromChannel(std::string const &nick)
{
	std::map<std::string, User *>::iterator it = _users.find(nick);
	if (it != _users.end())
		_users.erase(it);
	_kickedUsers.push_back(nick);
	size_t i = _nickList.find(nick);
	// std::cout << "nicklist bfore = " << _nickList << std::endl;
	if (i != std::string::npos)
		_nickList.erase(i, nick.size() + 1);
	// std::cout << "nicklist after = " << _nickList << std::endl;
}

void Channel::removeOperatorPrivilages(std::string const &nick)
{
	if (isOperator(nick))
		_operators.erase(std::find(_operators.begin(), _operators.end(), nick));
}

void Channel::addChannelMode(u_int8_t const &mode)
{
	_modes |= mode;
}

void Channel::removeChannelMode(u_int8_t const &mode)
{
	_modes ^= mode;
}
u_int8_t const &Channel::getChannelMode() const
{
	return (_modes);
}

void Channel::setUserLimit(int const &limit)
{
	_userLimit = limit;
}

std::string Channel::getChannelModeString()
{
	std::string mode;
	if (_modes & MODE_i)
		mode += 'i';
	if (_modes & MODE_t)
		mode += 't';
	if (_modes & MODE_k)
		mode += 'k';
	if (_modes & MODE_l)
		mode += 'l';
	//returning a string with a + in front of it if its not empty
	return (mode.empty() ? mode : "+" + mode);
}