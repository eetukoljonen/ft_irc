/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecution.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:29:04 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/24 14:52:13 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandExecution.hpp"
#include "Replies.hpp"

User *CommandExecution::_user;
Server *CommandExecution::_server;
Command CommandExecution::_command;

CommandExecution::CommandExecution(){}

CommandExecution::CommandExecution(CommandExecution const &cpy){(void)cpy;}

CommandExecution::~CommandExecution(){}

CommandExecution &CommandExecution::operator=(CommandExecution const &rhs)
{
    if (this != &rhs)
    {
    }
    return (*this);
}

void CommandExecution::execute(User	*user, Server *server, Command &command)
{
	_server = server;
	_user = user;
	_command = command;
	std::string	Cmds[8] = 
	{
		"JOIN",
		"NICK",
		"USER",
		"PASS",
		"CAP",
		"MOTD",
		"KILL",
		"MODE"
    };
    
    int i = 0;
	std::string _command = command.getCommand();
	std::cout << "commnad = " << _command << std::endl;
    while (i < 8)
    {
        if (!_command.compare(Cmds[i]))
            break;
        i++;
    }
	std::cout << "i = " << i << std::endl;
	switch (i)
	{
        case 0: _join(); break;
		case 1: _nick(); break;
		case 2: _userF(); break;
		case 3: _pass(); break;
		case 4: break ; //_cap(); break;
		case 5: _motd(); break;
		case 6: _kill(); break;
		case 7: _mode(); break;
		default: break;
		// 	addToClientBuffer(this, client_fd, ERR_UNKNOWNCOMMAND(client->getNickname(), cmd_infos.name));
	}
}

void CommandExecution::_join()
{
	if (!_user->isRegistered())
	{
		_user->addToSendBuffer(ERR_NOTREGISTERED(_server->getName()));
		return ;
	}
	std::vector<std::string> const &params = _command.getParams();
	size_t paramSize = params.size();
	if (paramSize == 0 || paramSize > 2)
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "JOIN"));
		return ;
	}
	std::vector<std::string> channelNames = split(params[0], ',');
	std::vector<std::string> channelKeys;
	if (paramSize == 2)
		channelKeys = split(params[1], ',');
	size_t channelCount = channelNames.size();
	size_t keyCount = channelKeys.size();
	if (keyCount > channelCount)
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "JOIN"));
		return ;
	}
	for (size_t i = 0; i < channelCount; i++)
	{
		if (channelNames[i].at(0) == '#' || channelNames[i].at(0) == '&')
			channelNames[i] = channelNames[i].substr(1);
		Channel *channel = _server->getChannelByName(channelNames[i]);
		// if channel exists
		if (channel != nullptr)
		{
			if (channel->isInviteOnly() && keyCount < i)
				_user->addToSendBuffer(ERR_INVITEONLYCHAN(_server->getName(), _user->getNick(), channelNames[i]));
			else if (channel->getUserCount() + 1 > channel->getUserLimit())
				_user->addToSendBuffer(ERR_CHANNELISFULL(_server->getName(), _user->getNick(), channel->getChannelName()));
			else
			{
				if (channel->getChannelkey().compare(channelKeys[i]))
					_user->addToSendBuffer(ERR_BADCHANNELKEY(_server->getName(), _user->getNick(), channelNames[i]));
				else
				{
					std::string const &topic = channel->getTopic();
					if (topic.empty())
						_user->addToSendBuffer(RPL_NOTOPIC(_server->getName(), _user->getNick(), channel->getChannelName()));
					else
						_user->addToSendBuffer(RPL_TOPIC(_server->getName(), _user->getNick(), channel->getChannelName(), channel->getTopic()));
					_user->addToSendBuffer(RPL_NAMES(_server->getName(), channel->getChannelName(), channel->getNickList(),  _user->getNick()));
					_user->addToSendBuffer(RPL_ENDOFNAMES(_server->getName(), channel->getChannelName(), _user->getNick()));
					channel->addToChannel(_user);
				}
			}
		}
		else // creating a new channel
		{
			if (!channelKeys.empty() && i <= keyCount)
				channel = _server->createChannel(channelNames[i], channelKeys[i]);
			else
				channel = _server->createChannel(channelNames[i]);
			channel->addToChannel(_user);
			_user->addToSendBuffer(RPL_NOTOPIC(_server->getName(), _user->getNick(), channel->getChannelName()));
			_user->addToSendBuffer(RPL_NAMES(_server->getName(), channel->getChannelName(), channel->getNickList(),  _user->getNick()));
			_user->addToSendBuffer(RPL_ENDOFNAMES(_server->getName(), channel->getChannelName(), _user->getNick()));
		}
	}
}

bool checkNick(std::string const &nick)
{
    // Regular expression for a valid IRC nickname
    std::regex nicknamePattern("^[A-Za-z[\\]^_`][A-Za-z0-9[\\]^_`]*$");
    // Check if the nickname matches the pattern
    return (std::regex_match(nick, nicknamePattern));
}

bool CommandExecution::_isValidNick()
{
	if (_command.getParams().empty())
	{
		_user->addToSendBuffer(ERR_NONICKNAMEGIVEN(_server->getName()));
		return (false);
	}
	if (_command.getParams().size() > 1)
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "NICK"));
		return (false);
	}
	std::string const &nick = _command.getParams().at(0);
	size_t nickSize = nick.size();
	if (nickSize == 0)
	{
		_user->addToSendBuffer(ERR_NONICKNAMEGIVEN(_server->getName()));
		return (false);
	}
	if (nickSize > 9)
	{
		_user->addToSendBuffer(ERR_ERRONEUSNICKNAME(_server->getName(), nick));
		return (false);
	}
	if (!checkNick(nick))
	{
		_user->addToSendBuffer(ERR_ERRONEUSNICKNAME(_server->getName(), nick));
		return (false);
	}
	if (_server->_findUserByNick(nick))
	{
		_user->addToSendBuffer(ERR_NICKNAMEINUSE(_server->getName(), nick));
		return (false);
	}
	return (true);
}

void CommandExecution::_nick()
{
	if (!_isValidNick())
		return ;
	_user->setNick(_command.getParams().at(0));
	if (!_user->isRegistered() && !_user->getUser().empty() && _user->isPassCorrect())
	{
		_user->setRegistrationFlag(true);
		_user->addToSendBuffer(RPL_WELCOME(_server->getName(), user_id(_user->getNick(), _user->getUser()), _user->getNick()));
	}
}

void CommandExecution::_motd()
{
	std::ifstream		data;

	data.open("motd"); // error handling
	std::cout << "data opened" << std::endl;
	data.close();
}


void CommandExecution::_userF()
{
	if (_command.getParams().empty())
	{
		std::cout << "adding to send buffer" << std::endl;
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), _command.getCommand()));
	}
	size_t size = _command.getParams().size();
	for (size_t i = 0; i < size; i++)
	{
		if (_command.getParams()[i].find(":") != std::string::npos)
		{
			std::string user = _command.getParams().at(i).substr(1, _command.getParams().at(i).size());
			_user->setUser(user);
			if (!_user->isRegistered() && !_user->getNick().empty() && _user->isPassCorrect())
			{
				_user->setRegistrationFlag(true);
				_user->addToSendBuffer(RPL_WELCOME(_server->getName(), user_id(_user->getNick(), _user->getUser()), _user->getNick()));
			}
			break ;
		}
	}
}

void CommandExecution::_channelMode()
{
	
}

void CommandExecution::_userMode()
{
	std::vector<std::string> const &cmdParams = _command.getParams();
	size_t parameters = cmdParams.size();
	if (parameters == 1)
	{
		_user->addToSendBuffer(RPL_UMODEIS(_server->getName(), _user->getNick(), _user->getUserMode()));
		return ;
	}
	if (parameters > 2)
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "MODE"));
		return ;
	}
	std::string mode = cmdParams.at(1);
	if (mode.size() != 2)
	{
		_user->addToSendBuffer(ERR_UMODEUNKNOWNFLAG(_server->getName(), _user->getNick(), mode));
		return ;
	}
	if (mode[0] == '-')
		_user->removeUserMode(&mode[1]);
	else if (mode[0] == '+')
		_user->addUserMode(&mode[1]);
	else
	{
		_user->addToSendBuffer(ERR_UMODEUNKNOWNFLAG(_server->getName(), _user->getNick(), mode));
		return ;
	}
	_user->addToSendBuffer(RPL_UMODEIS(_server->getName(), _user->getNick(), _user->getUserMode()));
}

void CommandExecution::_mode()
{
	std::vector<std::string> cmdParams = _command.getParams();
	if (cmdParams.empty())
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "PASS"));
		return ;
	}
	if (!cmdParams.at(0).compare(_user->getNick()))
		_userMode();
	
}

void CommandExecution::_pass()
{
	if (_command.getParams().empty())
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "PASS"));
		//kill
	}
	else if (!_command.getParams().empty() && 
	_command.getParams()[0].compare(_server->getPass())) // if pw does not match
	{
		_user->addToSendBuffer(ERR_PASSWDMISMATCH(_server->getName(), _user->getNick()));
		//kill connection
		return ;
	}
	_user->setPassFlag(true);
	if (!_user->isRegistered() && !_user->getNick().empty() && !_user->getUser().empty())
	{
		_user->setRegistrationFlag(true);
		_user->addToSendBuffer(RPL_WELCOME(_server->getName(), user_id(_user->getNick(), _user->getUser()), _user->getNick()));
	}
}

/*
Usage: An operator sends a KILL command followed by the nickname of the user to be disconnected and a reason for the disconnection.

Example: KILL target_nickname :Reason for kill
Server Action: Upon receiving a KILL command, the server will:
Validate that the sender has the necessary permissions to issue a KILL command.
Locate the user specified by the target_nickname.
Disconnect the targeted user from the server.
Optionally, propagate the KILL command to other servers if the IRC network is composed of multiple servers.
Client Notification: The targeted user's client is typically notified of the disconnection and the reason provided.
*/

void    CommandExecution::_kill()
{
    User *target;
	std::string reason;

	//validate permissions first

	if (!_command.getParams().empty())
	{
		target = _server->_findUserByNick(_command.getParams()[0]);
		if (target == nullptr)
		{
			_user->addToSendBuffer(ERR_NOSUCHNICK(_server->getName(), _user->getUser(), _command.getParams()[0]));
			return ;
		}
		else
		{	
			if (_command.getParams().size() > 1 && !_command.getParams()[1].empty())
			{
				if (!_command.getParams()[1].compare(":"))
					reason = ": default";
				else
					reason = _command.getParams()[1];
			}
			std::string msg = RPL_KILL(_server->getName(), target->getNick(), reason);
			if (send(target->getUserInfo().fd, msg.c_str(), msg.size(), 0) < 0)
				std::cout << "failed to send msg to" << target->getNick() 
				<< ". Error: " << strerror(errno) << std::endl;
			_server->deleteUser(target->getUserInfo().fd);
		}
	}
}

// void CommandExecution::_cap()
// {
// 	std::cout << "adding to send buffer" << std::endl;
// 	_user->addToSendBuffer("CAP * LS :\r\n");
// }}