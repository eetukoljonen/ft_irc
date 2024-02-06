/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecution.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:29:04 by ekoljone          #+#    #+#             */
/*   Updated: 2024/02/06 14:24:04 by ekoljone         ###   ########.fr       */
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
    (void)rhs;
	return (*this);
}

void CommandExecution::execute(User	*user, Server *server, Command &command)
{
	_server = server;
	_user = user;
	_command = command;
	std::string	Cmds[15] = 
	{
		"JOIN",
		"NICK",
		"USER",
		"PASS",
		"CAP",
		"MOTD",
		"KILL",
		"MODE",
		"KICK",
		"PING",
		"PONG",
		"INVITE",
		"PRIVMSG",
		"QUIT",
		"TOPIC"
    };
    
    int i = 0;
	std::string commandStr = command.getCommand();
    while (i < 15)
    {
        if (!commandStr.compare(Cmds[i]))
            break;
        i++;
    }
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
		case 8: _kick(); break;
		case 9: _ping(); break;
		case 10: _pong(); break;
		case 11: _invite(); break;
		case 12: _privmsg(); break;
		case 13: _quit(); break;
		case 14: _topic(); break;
		default: _user->addToSendBuffer(ERR_UNKNOWNCOMMAND(_server->getName(), _user->getNick(), _command.getCommand())); break;
	}
}

void CommandExecution::_joinSucces(Channel *channel)
{
	channel->addToChannel(_user);
	channel->broadcastToChannel(RPL_JOIN(user_id(_user->getNick(), _user->getUser(), _user->getIP()), "JOIN", channel->getChannelName()));
	std::string const &topic = channel->getTopic();
	if (!topic.empty())
		_user->addToSendBuffer(RPL_TOPIC(_server->getName(), _user->getNick(), channel->getChannelName(), channel->getTopic()));
	_user->addToSendBuffer(RPL_NAMES(_server->getName(), channel->getChannelName(), channel->getNickList(),  _user->getNick()));
	_user->addToSendBuffer(RPL_ENDOFNAMES(_server->getName(), channel->getChannelName(), _user->getNick()));
	_user->addNewChannel(channel);
}

void CommandExecution::_joinExistingChannel(Channel *channel, std::string const &key)
{
	if (channel->getChannelMode() & MODE_i && !channel->isInvited(_user->getNick()))
		_user->addToSendBuffer(ERR_INVITEONLYCHAN(_server->getName(), _user->getNick(), channel->getChannelName()));
	else if (channel->getUserCount() >= channel->getUserLimit())
		_user->addToSendBuffer(ERR_CHANNELISFULL(_server->getName(), _user->getNick(), channel->getChannelName()));
	else
	{
		if (channel->getChannelMode() & MODE_k && channel->getChannelkey().compare(key))
			_user->addToSendBuffer(ERR_BADCHANNELKEY(_server->getName(), _user->getNick(), channel->getChannelName()));
		else
			_joinSucces(channel);
	}
}

void CommandExecution::_joinNewChannel(std::string const &name)
{
	Channel *channel = _server->createChannel(name);
	channel->addToOperators(_user->getNick());
	_joinSucces(channel);
}

bool isValidChannelName(std::string const &channelName)
{
	if (channelName.empty() || channelName.size() >= 50)
		return (false);
	if (channelName.at(0) != '#' && channelName.at(0) != '&'
		&& channelName.at(0) != '!' && channelName.at(0) != '+')
		return (false);
	if (!checkIrcPattern(&channelName[1]))
		return (false);
	return (true);
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
	if (paramSize == 0)
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "JOIN"));
		return ;
	}
	std::vector<std::string> const &channelNames = split(params[0], ',');
	std::vector<std::string> channelKeys;
	if (paramSize >= 2)
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
		std::string channelName = channelNames[i];
		if (isValidChannelName(channelName))
		{
			std::string channelKey;
			if (i < keyCount)
				channelKey = channelKeys[i];
			channelName = channelNames[i].substr(1); //removing the prefix
			Channel *channel = _server->getChannelByName(channelName);
			// if channel exists
			if (channel != nullptr)
				_joinExistingChannel(channel, channelKey);
			else // creating a new channel
				_joinNewChannel(channelName);
		}
		else
			_user->addToSendBuffer(ERR_NOSUCHCHANNEL(_server->getName(), _user->getNick(), channelName));
	}
}

// bool checkIrcPattern(std::string const &str)
// {
//     // Regular expression for a valid IRC nickname
//     std::regex ircPattern("^[A-Za-z[\\]^_`][A-Za-z0-9[\\]^_`]*$");
//     // Check if the nickname matches the pattern
//     return (std::regex_match(str, ircPattern));
// }

bool CommandExecution::_isValidNick()
{
	if (_command.getParams().empty())
	{
		_user->addToSendBuffer(ERR_NONICKNAMEGIVEN(_server->getName()));
		return (false);
	}
	std::string nick = _command.getParams().at(0);
	size_t nickSize = nick.size();
	// if nickname len more than 15 we shrink it to 15
	if (nickSize > 15)
		nick = nick.substr(0, 15);
	if (!checkIrcPattern(nick))
	{
		_user->addToSendBuffer(ERR_ERRONEUSNICKNAME(_server->getName(), nick));
		return (false);
	}
	User *user = _server->_findUserByNick(nick);
	if (user)
	{
		if (user != _user)
			_user->addToSendBuffer(ERR_NICKNAMEINUSE(_server->getName(), nick));
		return (false);
	}
	return (true);
}

void CommandExecution::_nick()
{
	if (!_isValidNick())
		return ;
	std::string const	oldNick = _user->getNick();
	std::string	const	newNick = _command.getParams().at(0).substr(0, 15);
	_user->setNick(newNick);
	if (!_user->isRegistered() && !_user->getUser().empty() && _user->isPassCorrect())
	{
		_user->setRegistrationFlag(true);
		_user->addToSendBuffer(RPL_WELCOME(_server->getName(), user_id(_user->getNick(), _user->getUser(), _user->getIP()), _user->getNick()));
	}
	else if (_user->isRegistered())
	{
		if (!_user->getChannels().empty())
		{
			std::vector<Channel *>				_userChannels	= _user->getChannels();
			std::vector<Channel *>::iterator	it				= _userChannels.begin();
			std::vector<Channel *>::iterator	ite				= _userChannels.end();
			while (it != ite)
			{
				Channel *channel = *it;
				channel->removeFromChannel(oldNick);
				channel->addToChannel(_user);
				channel->broadcastToChannel(NICK(user_id(oldNick, _user->getUser(), _user->getIP()), _command.getParams().at(0)), _user);
				it++;
			}
		}
		_user->addToSendBuffer(NICK(user_id(oldNick, _user->getUser(), _user->getIP()), _command.getParams().at(0)));
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
	if (_user->isRegistered())
	{
		_user->addToSendBuffer(ERR_ALREADYREGISTERED(_server->getName()));
		return ;
	}
	std::vector<std::string> const &parameters = _command.getParams();
	size_t const &paramSize = parameters.size();
	if (paramSize < 4)
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), _command.getCommand()));
		return ;
	}
	std::string const &user = _command.getParams().at(0);
	_user->setUser(user);
	int mode = 0;
	try
	{
		mode = std::stoi(parameters[1]);
		if (mode == MODE_i)
		{
			_user->addToSendBuffer(USERMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), _user->getNick(), "+i"));
			_user->addUserMode("i");
		}
	}
	catch(const std::exception& e){}
	if (!_user->isRegistered() && !_user->getNick().empty() && _user->isPassCorrect())
	{
		_user->setRegistrationFlag(true);
		_user->addToSendBuffer(RPL_WELCOME(_server->getName(), user_id(_user->getNick(), _user->getUser(), _user->getIP()), _user->getNick()));
	}
}

void CommandExecution::_userMode()
{
	std::vector<std::string> const &cmdParams = _command.getParams();
	size_t parameters = cmdParams.size();
	std::string const &nick = cmdParams[0];
	if (nick.compare(_user->getNick()))
	{
		_user->addToSendBuffer(ERR_USERSDONTMATCH(_server->getName()));
		return ;
	}
	if (parameters == 1)
	{
		_user->addToSendBuffer(RPL_UMODEIS(_server->getName(), _user->getNick(), _user->getUserMode()));
		return ;
	}
	std::string mode = cmdParams.at(1);
	if (!mode.compare("+i") && _user->getUserMode().empty())
	{
		_user->addToSendBuffer(USERMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), _user->getNick(), mode));
		_user->addUserMode("i");
	}
}

void CommandExecution::_removeChannelModes(Channel *channel, std::string const &mode, std::string const &channelName, std::vector<std::string> const &modeParams)
{
	size_t paramIndex = 0;
	for (size_t i = 0; i < mode.size(); i++)
	{
		switch (mode[i])
		{
		case 'i':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (channel->getChannelMode() & MODE_i)
			{
				channel->removeChannelMode(MODE_i);
				channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, "-i"));
			}
			break ;
		case 'l':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (channel->getChannelMode() & MODE_l)
			{
				channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, "-l"));
				channel->removeChannelMode(MODE_l);
				channel->setUserLimit(32);
			}
			break ;
		case 't':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (channel->getChannelMode() & MODE_t)
			{
				channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, "-t"));
				channel->removeChannelMode(MODE_t);
			}
			break;
		case 'k':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (channel->getChannelMode() & MODE_k)
			{
				channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, "-k " + channel->getChannelkey()));
				channel->removeChannelMode(MODE_k);
			}
			break;
		case 'o':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (modeParams.size() <= paramIndex)
				_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "MODE"));
			else
			{
				// checking that user exist with this nickname on the whole server
				if (_server->_findUserByNick(modeParams[paramIndex]) == nullptr)
					_user->addToSendBuffer(ERR_NOSUCHNICK(_server->getName(), _user->getNick(), modeParams[paramIndex]));
				else if (!channel->UserOnChannel(modeParams[paramIndex])) // checking if on channel
					_user->addToSendBuffer(ERR_USERNOTONCHANNEL(_server->getName(), _user->getNick(), modeParams[paramIndex], channelName));
				else
				{
					// removing the op privilages
					channel->removeOperatorPrivilages(modeParams[paramIndex]);
					channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, "-o " + modeParams[paramIndex]));
					// _user->addToSendBuffer(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, mode[i]));
				}
				paramIndex++;
			}
			break ;
		default :
			_user->addToSendBuffer(ERR_UNKNOWNMODE(_server->getName(), mode[i], channelName));
			break ;
		}
	}
}

bool CommandExecution::_checkForOpPrivilages(Channel *channel)
{
	if (!channel->isOperator(_user->getNick()))
	{
		_user->addToSendBuffer(ERR_CHANOPRIVSNEEDED(_server->getName(), channel->getChannelName(), _user->getNick()));
		return (false);
	}
	return (true);
}

void CommandExecution::_addChannelModes(Channel *channel, std::string const &mode, std::string const &channelName, std::vector<std::string> const &modeParams)
{
	size_t paramIndex = 0;
	for (size_t i = 0; i < mode.size(); i++)
	{
		switch (mode[i])
		{
		case 'i':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (!(channel->getChannelMode() & MODE_i))
			{
				channel->addChannelMode(MODE_i);
				channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, "+i"));
			}
			break ;
		case 'l':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (modeParams.size() <= paramIndex)
				_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "MODE +l"));
			else 
			{
				int userLimit = 0;
				try{userLimit =  std::stoi(modeParams[paramIndex]);}
				catch(const std::exception& e){paramIndex++; break ;}
				if (userLimit != channel->getUserLimit())
				{
					channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, "+l " + std::to_string(userLimit)));
					channel->addChannelMode(MODE_l);
					channel->setUserLimit(userLimit);
				}
			}
			paramIndex++;
			break ;
		case 't':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (!(channel->getChannelMode() & MODE_t))
			{
				channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, "+t"));
				channel->addChannelMode(MODE_t);
			}
			break;
		case 'k':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (modeParams.size() <= paramIndex)
				_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "MODE +k"));
			else if (!(channel->getChannelMode() & MODE_k) || channel->getChannelkey().compare(modeParams[paramIndex]))
			{
				channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, "+k " + modeParams[paramIndex]));
				channel->addChannelMode(MODE_k);
				channel->setChannelKey(modeParams[paramIndex]);
			}
			paramIndex++;
			break;
		case 'o':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (modeParams.size() <= paramIndex)
				_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "MODE +o"));
			else
			{
				// checking that user exist with this nickname on the whole server
				if (_server->_findUserByNick(modeParams[paramIndex]) == nullptr)
					_user->addToSendBuffer(ERR_NOSUCHNICK(_server->getName(), _user->getNick(), modeParams[paramIndex]));
				else if (!channel->UserOnChannel(modeParams[paramIndex])) // checking if on channel
					_user->addToSendBuffer(ERR_USERNOTONCHANNEL(_server->getName(), _user->getNick(), modeParams[paramIndex], channelName));
				else
				{
					channel->addToOperators(modeParams[paramIndex]);
					channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, "+o " + modeParams[paramIndex]));
				}
				paramIndex++;
			}
			break ;
		default :
			_user->addToSendBuffer(ERR_UNKNOWNMODE(_server->getName(), mode[i], channelName));
			break ;
		}
	}
}

void CommandExecution::_channelMode()
{
	std::vector<std::string> const &cmdParams = _command.getParams();
	size_t const &paramSize = cmdParams.size();
	std::string const channelName = cmdParams[0];
	Channel *channel = _server->getChannelByName(&channelName[1]);
	if (!isValidChannelName(channelName) || !channel)
	{
		_user->addToSendBuffer(ERR_NOSUCHCHANNEL(_server->getName(), _user->getNick(), channelName));
		return ;
	}
	if (paramSize == 1)
	{
		_user->addToSendBuffer(RPL_CHANNELMODIS(_server->getName(), _user->getNick(), channelName, channel->getChannelModeString()));
		return ;
	}
	std::string const &mode = cmdParams[1];
	// making a new vector for the mode parameters
	std::vector<std::string> modeParams(cmdParams.begin() + 2, cmdParams.end());
	char prefix = mode[0];
	if (prefix == '-')
		_removeChannelModes(channel, &mode[1], channelName, modeParams);
	else if (prefix == '+')
		_addChannelModes(channel, &mode[1], channelName, modeParams);
}

void CommandExecution::_mode()
{
	std::vector<std::string> const &cmdParams = _command.getParams();
	if (cmdParams.empty())
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "MODE"));
		return ;
	}
	if (cmdParams.at(0)[0] != '#' && cmdParams.at(0)[0] != '&' && cmdParams.at(0)[0] != '!' && cmdParams.at(0)[0] != '+')
		_userMode();
	else
		_channelMode();
}

void CommandExecution::_pass()
{
	if (_user->isRegistered())
	{
		_user->addToSendBuffer(ERR_ALREADYREGISTERED(_server->getName()));
		return ;
	}
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
		_user->addToSendBuffer(RPL_WELCOME(_server->getName(), user_id(_user->getNick(), _user->getUser(), _user->getIP()), _user->getNick()));
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
				std::cout << "failed to send msg to" << target->getNick() \
				<< ". Error: " << strerror(errno) << std::endl;
			_server->deleteUser(target->getUserInfo().fd);
		}
	}
}

void	CommandExecution::_kick()
{
	std::string const &serverName = _server->getName();
	std::string const &userName = _user->getUser();
	std::string const &command = _command.getCommand();
	std::string const &kickerNick = _user->getNick();
		
	/* Check that the command has enough params */
	if (_command.getParams().size() < 2)
	{
    	_user->addToSendBuffer(ERR_NEEDMOREPARAMS(serverName, kickerNick, command));
    	return;
	}
	
	//channel name without the hashtag -> added it back in the replies.hpp
	std::string const &channelName = &_command.getParams()[0][1]; 
	std::string const &targetUser = _command.getParams()[1];
	Channel *channel = _server->getChannelByName(channelName);
	// User *target = channel->getUser(targetUser);
	std::string reason;
	/* Check channel exists */
	if (channel == nullptr)
	{
		_user->addToSendBuffer(ERR_NOSUCHCHANNEL(serverName, userName, channelName));
		return ;
	}
	/* Check channel name */
	if (!isValidChannelName(_command.getParams()[0]))
	{
		_user->addToSendBuffer(ERR_BADCHANMASK(serverName, userName, _command.getParams()[0]));
		return ;
	}
	/* Check user privileges */
	if (!channel->isOperator(kickerNick))
	{
		_user->addToSendBuffer(ERR_CHANOPRIVSNEEDED(serverName, channelName, channelName));
		return ;
	}
	/* Check that kicker is on channel */
	if (!channel->UserOnChannel(kickerNick))
	{
		_user->addToSendBuffer(ERR_NOTONCHANNEL(serverName, userName, channelName));
		return ;
	}
	/* Check that kicked is on channel */
	if (!channel->UserOnChannel(targetUser))
	{
		_user->addToSendBuffer(ERR_USERNOTONCHANNEL(serverName, kickerNick, targetUser, channelName));
		return ;
	}
	if (_command.getParams().size() > 2 && _command.getParams()[2].compare(":"))
		reason = _command.getParams()[2];
	else
		reason = "default";
	// std::string const &msg = RPL_KICKEDCLIENT(serverName, kickerNick, \
	// 									channelName, target->getNick(), reason);
	// if (send(target->getUserInfo().fd, msg.c_str(), msg.size(), 0) < 0)
   	// 	std::cerr << "Error sending message: " << strerror(errno) << std::endl;
	channel->broadcastToChannel(RPL_KICKBROADCAST(user_id(kickerNick, userName, _user->getIP()), channelName, targetUser, reason));
	channel->removeFromChannel(targetUser);
}

void	CommandExecution::_invite()
{
	std::string const &serverName = _server->getName();
	std::string const &userName = _user->getUser();
	std::string const &command = _command.getCommand();
	std::string const &inviterNick = _user->getNick();
	
	if (_command.getParams().size() < 2) 
	{
        _user->addToSendBuffer(ERR_NEEDMOREPARAMS(serverName, inviterNick, command));
        return;
    }

    std::string const &targetNick = _command.getParams()[0];
    std::string const &channelName = &_command.getParams()[1][1];
	Channel *channel = _server->getChannelByName(channelName);
	/* Check channel name */
	if (!isValidChannelName(_command.getParams()[1]))
	{
		_user->addToSendBuffer(ERR_BADCHANMASK(serverName, userName, channelName));
		return ;
	}
	/* Check channel exists */
	if (channel == nullptr)
	{
		_user->addToSendBuffer(ERR_NOSUCHCHANNEL(serverName, userName, channelName));
		return ;
	}
	/* Check user privileges */
	if (!channel->isOperator(inviterNick))
	{
		_user->addToSendBuffer(ERR_CHANOPRIVSNEEDED(serverName, inviterNick, channelName));
		return ;
	}
	/* Check that inviter is on channel */
	if (!channel->UserOnChannel(inviterNick))
	{
		_user->addToSendBuffer(ERR_NOTONCHANNEL(serverName, userName, channelName));
		return ;
	}
	/* Check that invited is NOT on channel */
	if (channel->UserOnChannel(targetNick))
	{
		_user->addToSendBuffer(ERR_USERONCHANNEL(serverName, inviterNick, targetNick, channelName));
		return ;
	}
	User *target = _server->_findUserByNick(targetNick);
	if (target == nullptr)
	{
		_user->addToSendBuffer(ERR_NOSUCHNICK(serverName, userName, targetNick));
		return ;
	}
	std::string const &inviteMsg = RPL_INVITE(inviterNick, userName, serverName, targetNick, channelName);
	target->addToSendBuffer(inviteMsg);
	channel->addToInviteList(targetNick);
}

void CommandExecution::_pong()
{
	std::vector<std::string> const &parameters = _command.getParams();
	if (parameters.empty() || parameters.size() > 1)
		return ;
	std::string response = parameters[0];
	if (response[0] == ':')
		response = response.substr(1);
	if (!response.compare(_user->getPongResponse()))
		_user->resetPingResponseTimer();
}

void CommandExecution::_ping()
{
	if (_command.getParams().empty())
	{
		_user->addToSendBuffer(ERR_NOORIGIN(_server->getName()));
		return ;
	}
	std::string msg = _command.getParams()[0];
	if (msg[0] == ':')
		msg = msg.substr(1);
	if (msg.empty())
	{
		_user->addToSendBuffer(ERR_NOORIGIN(_server->getName()));
		return ;
	}
	_user->addToSendBuffer(PONG(_server->getName(), msg));
}

// PRIVMSG <receiver> :<message>

void CommandExecution::_privmsg()
{
	std::string const &serverName = _server->getName();
	std::string const &userName = _user->getUser();
	std::string const &command = _command.getCommand();
	std::string const &userNick = _user->getNick();
	
	if (_command.getParams().size() < 2) 
	{
        _user->addToSendBuffer(ERR_NEEDMOREPARAMS(serverName, userNick, command));
        return;
    }

	std::string const &receiver = _command.getParams()[0];
	std::string const &msg = _command.getParams()[1];
	if (msg.find(":") == std::string::npos)
	{
		_user->addToInputBuffer(ERR_NOTEXTTOSEND(userNick));
	}
	
	// std::cout << "receiver is " << receiver << std::endl;
	// std::cout << "privmsg is " << msg << std::endl;

	Channel* channel = _server->getChannelByName(&receiver[1]);
	//todo add bad channel mask check
	
	if (!receiver.empty() && receiver[0] == '#')
	{
		// The receiver is a channel
		if (channel) 
		{
			// std::cout << "hello there\n";
			if (channel->UserOnChannel(userNick))
				channel->broadcastToChannel(RPL_PRIVMSG(userNick, userName, receiver, msg), _user);
			else 
				_user->addToSendBuffer(ERR_NOSUCHNICK(serverName, userName, userNick));
		}
		else
			_user->addToSendBuffer(ERR_NOSUCHCHANNEL(serverName, userName, receiver));
	} 
	else 
	{
		// The receiver is a user
		User* recipient = _server->_findUserByNick(receiver);
		// std::cout << "user is " << recipient->getNick() << std::endl;

		if (recipient)
			recipient->addToSendBuffer(RPL_PRIVMSG(userNick, userName, receiver, msg));
		else
			_user->addToSendBuffer(ERR_NOSUCHNICK(serverName, userName, userNick));
	}	
}

void CommandExecution::_quit()
{
	std::vector<Channel *>					_usersChannels	= _user->getChannels();
	if (!_usersChannels.empty())
	{
		std::vector<Channel *>::iterator	it				= _usersChannels.begin();
		std::vector<Channel *>::iterator	ite				= _usersChannels.end();
		std::string msg = ":leaving";
		if (!_command.getParams().empty())
			msg	= _command.getParams().at(0);
		std::string const					&nick			= _user->getNick();
		std::string const					&user			= _user->getUser();
		std::string const					&userIP			= _user->getIP();
		while (it != ite)
		{
			Channel *channel = *it;
			channel->removeFromChannel(nick);
			channel->broadcastToChannel(QUIT(user_id(nick, user, userIP), msg));
			it++;
		}
	}
	_server->deleteUser(_user->getUserInfo().fd);
	// todo check deleteUser function
}

void CommandExecution::_topic()
{
	std::vector<std::string> const &cmdParams = _command.getParams();
	size_t paramSize = cmdParams.size();
	if (paramSize == 0)
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "TOPIC"));
		return ;
	}
	std::string channelName = cmdParams[0];
	if (!isValidChannelName(channelName))
	{
		_user->addToSendBuffer(ERR_BADCHANMASK(_server->getName(), _user->getNick(), channelName));
		return ;
	}
	channelName = channelName.substr(1);
	Channel *channel = _server->getChannelByName(channelName);
	if (channel == nullptr)
	{
		_user->addToSendBuffer(ERR_NOSUCHCHANNEL(_server->getName(), _user->getNick(), channelName));
		return ;
	}
	if (paramSize == 1)
	{
		if (channel->getTopic().empty())
			_user->addToSendBuffer(RPL_NOTOPIC(_server->getName(), _user->getNick(), channelName));
		else
			_user->addToSendBuffer(RPL_TOPIC(_server->getName(), _user->getNick(), channelName, channel->getTopic()));
		return ;
	}
	if (channel->getChannelMode() & MODE_t && !channel->isOperator(_user->getNick()))
	{
		_user->addToSendBuffer(ERR_CHANOPRIVSNEEDED(_server->getName(), channelName, _user->getNick()));
		return ;
	}
	std::string topic = cmdParams[1];
	if (!topic.empty() && topic[0] == ':')
		topic = topic.substr(1);
	if (topic.empty())
		channel->clearTopic();
	else
		channel->setTopic(topic);
	if (channel->getTopic().empty())
		_user->addToSendBuffer(RPL_NOTOPIC(_server->getName(), _user->getNick(), channelName));
	else
		_user->addToSendBuffer(RPL_TOPIC(_server->getName(), _user->getNick(), channelName, channel->getTopic()));
}