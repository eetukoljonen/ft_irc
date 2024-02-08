/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 10:53:28 by ekoljone          #+#    #+#             */
/*   Updated: 2024/02/08 11:18:38 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel() : _userLimit(32), _modes(0) {}

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

std::string Channel::getNickList()
{
	std::string nicklist;
	std::map<std::string, User *>::const_iterator it = _users.cbegin();
	std::map<std::string, User *>::const_iterator ite = _users.cend();
	while (it != ite)
	{
		if (!nicklist.empty())
			nicklist += " ";
		if (isOperator(it->first))
			nicklist +=  "@" + it->first;
		else
			nicklist += it->first;
		it++;
	}
	return (nicklist);
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

// here we send the msg to everyone on the channel except the ignoredUser, usually
// the ignoredUser will be the sender of the msg, PRIVMSG for example
void Channel::broadcastToChannel(std::string const &msg, User *ignoredUser) 
{
	std::map<std::string, User *>::iterator it;
	if (!_users.empty())
	{
		for(it = _users.begin(); it != _users.end(); ++it)
		{
			if (it->second != ignoredUser)
				it->second->addToSendBuffer(msg);
		}
	}
}

void Channel::broadcastToChannel(std::string const &msg) 
{
	std::map<std::string, User *>::iterator it;
	if (!_users.empty())
	{
		for(it = _users.begin(); it != _users.end(); ++it)
       		it->second->addToSendBuffer(msg);
	}
}

void	Channel::removeFromChannel(std::string const &nick)
{
	std::map<std::string, User *>::iterator it = _users.find(nick);
	if (it != _users.end())
		_users.erase(it);
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

void Channel::clearTopic()
{
	_topic.clear();
}

void Channel::setTopic(std::string const &topic)
{
	_topic = topic;
}
