/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecution.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atuliara <atuliara@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:29:04 by ekoljone          #+#    #+#             */
/*   Updated: 2024/02/14 15:26:12 by atuliara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/CommandExecution.hpp"
#include "../headers/Replies.hpp"

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
	std::string	Cmds[17] = 
	{
		"JOIN",
		"NICK",
		"USER",
		"PASS",
		"CAP",
		"MOTD",
		"MODE",
		"KICK",
		"PING",
		"PONG",
		"INVITE",
		"PRIVMSG",
		"QUIT",
		"TOPIC",
		"PART",
		"WHO",
		"WHOIS"
    };
    
    int i = 0;
	std::string const &commandStr = command.getCommand();
    while (i < 17)
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
		case 4: break ; // cap break to ignore 
		case 5: _motd(); break;
		case 6: _mode(); break;
		case 7: _kick(); break;
		case 8: _ping(); break;
		case 9: _pong(); break;
		case 10: _invite(); break;
		case 11: _privmsg(); break;
		case 12: _quit(); break;
		case 13: _topic(); break;
		case 14: _part(); break;
		case 15: _who(); break;
		case 16: break; // whois break to ignore
		default: _user->addToSendBuffer(ERR_UNKNOWNCOMMAND(_server->getName(), _user->getNick(), _command.getCommand())); break;
	}
}

void CommandExecution::_joinSucces(Channel *channel)
{
	channel->addToChannel(_user);
	channel->broadcastToChannel(RPL_JOIN(user_id(_user->getNick(), _user->getUser(), _user->getIP()), "JOIN", "#" + channel->getChannelName()));
	std::string const &topic = channel->getTopic();
	if (!topic.empty())
		_user->addToSendBuffer(RPL_TOPIC(_server->getName(), _user->getNick(), "#" + channel->getChannelName(), channel->getTopic()));
	_user->addToSendBuffer(RPL_NAMES(_server->getName(), "#" + channel->getChannelName(), channel->getNickList(),  _user->getNick()));
	_user->addToSendBuffer(RPL_ENDOFNAMES(_server->getName(), "#" + channel->getChannelName(), _user->getNick()));
	_user->addNewChannel(channel);
}

void CommandExecution::_joinExistingChannel(Channel *channel, std::string const &key)
{
	if (channel->UserOnChannel(_user->getNick()))
		return ;
	if (channel->getChannelMode() & MODE_i && !channel->isInvited(_user->getNick()))
		_user->addToSendBuffer(ERR_INVITEONLYCHAN(_server->getName(), _user->getNick(), "#" + channel->getChannelName()));
	else if (channel->getUserCount() >= channel->getUserLimit())
		_user->addToSendBuffer(ERR_CHANNELISFULL(_server->getName(), _user->getNick(), "#" + channel->getChannelName()));
	else if (channel->getChannelMode() & MODE_i && channel->isInvited(_user->getNick()))
		_joinSucces(channel);
	else if (channel->getChannelMode() & MODE_k && channel->getChannelkey().compare(key))
		_user->addToSendBuffer(ERR_BADCHANNELKEY(_server->getName(), _user->getNick(), "#" + channel->getChannelName()));
	else
		_joinSucces(channel);
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
	if (channelName.at(0) != '#')
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
	std::vector<std::string> const &channelNames = split(params[0], ',', false);
	std::vector<std::string> channelKeys;
	if (paramSize >= 2)
		channelKeys = split(params[1], ',', false);
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
	if (!_user->isPassCorrect())
	{
		_user->addToSendBuffer(ERR_NOTREGISTERED(_server->getName()));
		_user->restrictUser();
		return ;
	}
	std::string const	oldNick = _user->getNick();
	std::string	const	newNick = _command.getParams().at(0).substr(0, 15);
	_user->setNick(newNick);
	if (!_user->isRegistered() && !_user->getUser().empty() && _user->isPassCorrect())
	{
		_user->setRegistrationFlag(true);
		_user->addToSendBuffer(RPL_WELCOME(_server->getName(), user_id(_user->getNick(), _user->getUser(), _user->getIP()), _user->getNick()));
		_motd();
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
				channel->broadcastToChannel(NICK(user_id(oldNick, _user->getUser(), _user->getIP()), newNick), _user);
				it++;
			}
		}
		_user->addToSendBuffer(NICK(user_id(oldNick, _user->getUser(), _user->getIP()), newNick));
	}
}

static std::string readFileIntoString(const std::string& path) 
{
    std::ifstream fileStream(path);
    std::stringstream buffer;

    if (fileStream)
        buffer << fileStream.rdbuf();	
    return buffer.str();
}
void CommandExecution::_motd()
{
    std::string serverName = _server->getName();
    std::string userName = _user->getUser();
    std::string replyStart = RPL_MOTDSTART(serverName, userName);
    _user->addToSendBuffer(replyStart);
    std::string motd = readFileIntoString("motd.txt");
    std::istringstream motdStream(motd);
    std::string line;

    while (std::getline(motdStream, line)) 
	{
        if (!line.empty() && line.length() < 80) 
		{
            std::string replyLine = RPL_MOTD(serverName, userName, line);
            _user->addToSendBuffer(replyLine);
        }
    }
    if (!motd.empty()) 
	{
        std::string replyEnd = RPL_ENDOFMOTD(serverName, userName);
        _user->addToSendBuffer(replyEnd);
    }
	else 
	{
        std::string replyNoMotd = ERR_NOMOTD(serverName, userName);
        _user->addToSendBuffer(replyNoMotd);
    }
}

void CommandExecution::_userF()
{
	if (_user->isRegistered())
	{
		_user->addToSendBuffer(ERR_ALREADYREGISTERED(_server->getName()));
		return ;
	}
	if (!_user->isPassCorrect())
	{
		_user->addToSendBuffer(ERR_NOTREGISTERED(_server->getName()));
		_user->restrictUser();
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
		_motd();
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
	std::string modeSTR = "-";
	std::string paramSTR;
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
				modeSTR += "i";
			}
			break ;
		case 'l':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (channel->getChannelMode() & MODE_l)
			{
				modeSTR += "l";
				channel->removeChannelMode(MODE_l);
				channel->setUserLimit(MAX_CLIENTS);
			}
			break ;
		case 't':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (channel->getChannelMode() & MODE_t)
			{
				modeSTR += "t";
				channel->removeChannelMode(MODE_t);
			}
			break;
		case 'k':
			if (!_checkForOpPrivilages(channel))
				break ;
			if (channel->getChannelMode() & MODE_k)
			{
				modeSTR += "k";
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
					if (!paramSTR.empty())
						paramSTR += " ";
					paramSTR += modeParams[paramIndex];
					modeSTR += "o";
				}
				paramIndex++;
			}
			break ;
		default :
			_user->addToSendBuffer(ERR_UNKNOWNMODE(_server->getName(), mode[i], channelName));
			break ;
		}
	}
	if (!paramSTR.empty())
		modeSTR += " ";
	if (modeSTR.size() > 1)
		channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, modeSTR + paramSTR));
}

bool CommandExecution::_checkForOpPrivilages(Channel *channel)
{
	if (!channel->isOperator(_user->getNick()))
	{
		_user->addToSendBuffer(ERR_CHANOPRIVSNEEDED(_server->getName(), "#" + channel->getChannelName(), _user->getNick()));
		return (false);
	}
	return (true);
}

void CommandExecution::_addChannelModes(Channel *channel, std::string const &mode, std::string const &channelName, std::vector<std::string> const &modeParams)
{
	size_t paramIndex = 0;
	std::string modeSTR = "+";
	std::string paramSTR;
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
				modeSTR += "i";
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
					modeSTR += "l";
					if (!paramSTR.empty())
						paramSTR += " ";
					paramSTR += std::to_string(userLimit);
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
				modeSTR += "t";
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
				modeSTR += "k";
				if (!paramSTR.empty())
					paramSTR += " ";
				paramSTR += modeParams[paramIndex];
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
					modeSTR += "o";
					if (!paramSTR.empty())
						paramSTR += " ";
					paramSTR += modeParams[paramIndex];
				}
				paramIndex++;
			}
			break ;
		default :
			_user->addToSendBuffer(ERR_UNKNOWNMODE(_server->getName(), mode[i], channelName));
			break ;
		}
	}
	if (!paramSTR.empty())
		modeSTR += " ";
	if (modeSTR.size() > 1)
		channel->broadcastToChannel(CHANNELMODE(user_id(_user->getNick(), _user->getUser(), _user->getIP()), channelName, modeSTR + paramSTR));
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
	else if (!mode.compare("b"))
		_user->addToSendBuffer(RPL_ENDOFBANLIST(_server->getName(), _user->getNick(), channelName));
	else
		_user->addToSendBuffer(ERR_UNKNOWNMODE(_server->getName(), mode, channelName));
}

void CommandExecution::_mode()
{
	if (!_user->isRegistered())
	{
		_user->addToSendBuffer(ERR_NOTREGISTERED(_server->getName()));
		return ;
	}
	std::vector<std::string> const &cmdParams = _command.getParams();
	if (cmdParams.empty())
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "MODE"));
		return ;
	}
	if (cmdParams.at(0)[0] != '#')
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
		_user->restrictUser();
		return  ;
	}
	else if (!_command.getParams().empty()
			&& _command.getParams()[0].compare(_server->getPass()))
	{
		_user->addToSendBuffer(ERR_PASSWDMISMATCH(_server->getName(), _user->getNick()));
		_user->restrictUser();
		return ;
	}
	_user->setPassFlag(true);
}

void	CommandExecution::_kick()
{
	if (!_user->isRegistered())
	{
		_user->addToSendBuffer(ERR_NOTREGISTERED(_server->getName()));
		return ;
	}
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
	/* Loop over all the channels and targetUsers */
	std::vector<std::string> channelNames = split(_command.getParams()[0], ',', false);
	std::vector<std::string> targetUsers = split(_command.getParams()[1], ',', false);
	for (const std::string &channelName : channelNames)
	{
		Channel *channel = _server->getChannelByName(&channelName[1]);
		/* Check channel exists */
		if (channel == nullptr || !isValidChannelName(channelName))
		{
			_user->addToSendBuffer(ERR_NOSUCHCHANNEL(serverName, userName, channelName));
		}
		else if (!channel->isOperator(kickerNick))
		{
			_user->addToSendBuffer(ERR_CHANOPRIVSNEEDED(serverName, kickerNick, channelName));
		}
		/* Check that kicker is on channel */
		else if (!channel->UserOnChannel(kickerNick))
		{
			_user->addToSendBuffer(ERR_NOTONCHANNEL(serverName, userName, channelName));
		}
		else
		{
			for (const std::string &targetUser : targetUsers)
			{
				/* Check that kicked is on channel */
				if (!channel->UserOnChannel(targetUser))
				{
					_user->addToSendBuffer(ERR_USERNOTONCHANNEL(serverName, kickerNick, targetUser, channelName));
					break ;
				}
				std::string reason;
				if (_command.getParams().size() > 2 && _command.getParams()[2].compare(":"))
					reason = _command.getParams()[2];
				else
					reason = "default";
				channel->broadcastToChannel(RPL_KICKBROADCAST(user_id(kickerNick, userName, _user->getIP()), channelName, targetUser, reason));
				channel->removeFromChannel(targetUser);
				_user->removeChannel(channel);
				if (channel->getUserCount() <= 0)
					_server->deleteChannel(channel);
			}
			
		}
			
	}
}

void	CommandExecution::_invite()
{
	if (!_user->isRegistered())
	{
		_user->addToSendBuffer(ERR_NOTREGISTERED(_server->getName()));
		return ;
	}
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
    std::string const &channelName = _command.getParams()[1];
	Channel *channel = _server->getChannelByName(&channelName[1]);
	/* Check channel name */
	if (!isValidChannelName(_command.getParams()[1]))
	{
		_user->addToSendBuffer(ERR_BADCHANMASK(serverName, userName, _command.getParams()[1]));
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
		_user->addToSendBuffer(ERR_CHANOPRIVSNEEDED(serverName, channelName, inviterNick));
		return ;
	}
	/* Check that inviter is on channel */
	if (!channel->UserOnChannel(inviterNick))
	{
		_user->addToSendBuffer(ERR_NOTONCHANNEL(serverName, inviterNick, channelName));
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
	std::string const &inviteMsg = RPL_INVITE(inviterNick, userName, _user->getIP(), targetNick, channelName);
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

void CommandExecution::_privmsg()
{
	if (!_user->isRegistered())
	{
		_user->addToSendBuffer(ERR_NOTREGISTERED(_server->getName()));
		return ;
	}
	std::string const &serverName = _server->getName();
	std::string const &userName = _user->getUser();
	std::string const &command = _command.getCommand();
	std::string const &userNick = _user->getNick();
	
	if (_command.getParams().size() < 2) 
	{
        _user->addToSendBuffer(ERR_NEEDMOREPARAMS(serverName, userNick, command));
        return;
    }
	std::string const &msg = _command.getParams()[1];
	if (msg.find(":") == std::string::npos)
	{
		_user->addToInputBuffer(ERR_NOTEXTTOSEND(userNick));
	}
	//loop over all the receivers
	std::vector<std::string> receivers = split(_command.getParams()[0], ',', false);
	for (const std::string &receiver : receivers)
	{
		if (!receivers.empty() && isValidChannelName(receiver))
		{
			Channel* channel = _server->getChannelByName(&receiver[1]);
			// The receiver is a channel
			if (channel) 
			{
				if (channel->UserOnChannel(userNick))
					channel->broadcastToChannel(RPL_PRIVMSG(user_id(userNick, userName, _user->getIP()), receiver, msg), _user);
				else 
					_user->addToSendBuffer(ERR_NOSUCHNICK(serverName, userNick, userNick));
			} 
			else
				_user->addToSendBuffer(ERR_NOSUCHCHANNEL(serverName, userName, receiver));
		}
		else
		{
			// The receiver is a user
			User* recipient = _server->_findUserByNick(receiver);
			if (recipient)
				recipient->addToSendBuffer(RPL_PRIVMSG(user_id(userNick, userName, _user->getIP()), receiver, msg));
			else
				_user->addToSendBuffer(ERR_NOSUCHNICK(serverName, userNick, receiver));
		}	
	}
}

void CommandExecution::_quit()
{
	std::vector<Channel *>					_usersChannels	= _user->getChannels();
	if (!_usersChannels.empty())
	{
		std::vector<Channel *>::iterator	it				= _usersChannels.begin();
		std::string msg = ":leaving";
		if (!_command.getParams().empty())
			msg	= _command.getParams().at(0);
		std::string const					&nick			= _user->getNick();
		std::string const					&user			= _user->getUser();
		std::string const					&userIP			= _user->getIP();
		while (it != _usersChannels.end())
		{
			Channel *channel = *it;
			it++;
			channel->removeFromChannel(nick);
			channel->broadcastToChannel(QUIT(user_id(nick, user, userIP), msg));
			if (channel->getUserCount() <= 0)
				_server->deleteChannel(channel);
		}
	}
	_server->deleteUser(_user->getUserInfo().fd);
}

void CommandExecution::_topic()
{
	if (!_user->isRegistered())
	{
		_user->addToSendBuffer(ERR_NOTREGISTERED(_server->getName()));
		return ;
	}
	std::vector<std::string> const &cmdParams = _command.getParams();
	size_t paramSize = cmdParams.size();
	if (paramSize == 0)
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "TOPIC"));
		return ;
	}
	std::string const &channelName = cmdParams[0];
	Channel *channel = _server->getChannelByName(&channelName[1]);
	if (!isValidChannelName(channelName) || channel == nullptr)
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
	channel->broadcastToChannel(TOPIC(user_id(_user->getNick(), _user->getUser(), _user->getIP()), "#" + channel->getChannelName(), topic));
}

void CommandExecution::_part()
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
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "PART"));
		return ;
	}
	std::vector<std::string> const &channelNames = split(params[0], ',', false);
	std::vector<std::string> partMessages;
	if (paramSize > 1)
		partMessages = std::vector<std::string>(params.begin() + 1, params.end());
	size_t channelCount = channelNames.size();
	for (size_t i = 0; i < channelCount; i++)
	{
		std::string channelName = channelNames[i];
		if (isValidChannelName(channelName))
		{
			channelName = channelNames[i].substr(1); //removing the prefix
			Channel *channel = _server->getChannelByName(channelName);
			// if channel exists
			if (channel != nullptr)
			{
				if (channel->UserOnChannel(_user->getNick()))
				{
					std::string reason;
					if (partMessages.size() > i)
						reason = partMessages[i];
					if (reason.empty() || (reason.size() > 1 && reason[0] != ':'))
						reason = ":" + reason;
					channel->broadcastToChannel(PART(user_id(_user->getNick(), _user->getUser(), _user->getIP()), "#" + channelName, reason));
					_user->removeChannel(channel);
					channel->removeFromChannel(_user->getNick());
					if (channel->getUserCount() <= 0)
						_server->deleteChannel(channel);
				}
				else
					_user->addToSendBuffer(ERR_NOTONCHANNEL(_server->getName(), _user->getNick(), channelName));
			}
			else
				_user->addToSendBuffer(ERR_NOSUCHCHANNEL(_server->getName(), _user->getNick(), channelName));
		}
		else
			_user->addToSendBuffer(ERR_NOSUCHCHANNEL(_server->getName(), _user->getNick(), channelName));
	}
}

void CommandExecution::_who()
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
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_server->getName(), _user->getNick(), "PART"));
		return ;
	}
	std::string const &channelName = params[0];
	if (!isValidChannelName(channelName))
	{
		_user->addToSendBuffer(ERR_NOSUCHCHANNEL(_server->getName(), _user->getNick(), channelName));
		return ;
	}
	_user->addToSendBuffer(RPL_ENDOFWHO(_server->getName(), _user->getNick(), channelName));
}
