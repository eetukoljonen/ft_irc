/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecution.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:29:04 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/22 17:08:43 by ekoljone         ###   ########.fr       */
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
	std::string	Cmds[7] = 
	{
		"JOIN",
		"NICK",
		"USER",
		"PASS",
		"CAP",
		"MOTD",
		"KILL"
    };
    
    int i = 0;
	std::string _command = command.getCommand();
	std::cout << "commnad = " << _command << std::endl;
    while (i < 6)
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
		default: break;
		// 	addToClientBuffer(this, client_fd, ERR_UNKNOWNCOMMAND(client->getNickname(), cmd_infos.name));
	}
}

void CommandExecution::_join()
{
	if (!_user->isRegistered())
	{
		std::cout << "adding to send buffer" << std::endl;
		_user->addToSendBuffer(ERR_NOTREGISTERED(_server->getName()));
	}
}

void CommandExecution::_nick()
{
	if (_command.getParams().empty())
	{
		_user->addToSendBuffer(":Garbaggio 431 * :No nickname given\r\n");
		return ;
	}
	if (_command.getParams().size() > 1)
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_user->getNick(), "NICK"));
		return ;
	}
	_user->setNick(_command.getParams().at(0));
	if (!_user->isRegistered() && !_user->getUser().empty() && _user->isPassCorrect())
	{
		_user->setRegistrationFlag(true);
		_user->addToSendBuffer(RPL_WELCOME(user_id(_user->getNick(), _user->getUser()), _user->getNick()));
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
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_user->getNick(), _command.getCommand()));
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
				_user->addToSendBuffer(RPL_WELCOME(user_id(_user->getNick(), _user->getUser()), _user->getNick()));
			}
			break ;
		}
	}
}

void CommandExecution::_pass()
{
	if (_command.getParams().empty())
	{
		_user->addToSendBuffer(ERR_NEEDMOREPARAMS(_user->getNick(), "PASS"));
		//kill
	}
	else if (!_command.getParams().empty() && 
	!_command.getParams()[0].compare(_server->getPass())) // if pw does not match
	{
		_user->addToSendBuffer(ERR_PASSWDMISMATCH(_user->getNick()));
		//kill connection
		return ;
	}
	_user->setPassFlag(true);
	if (!_user->isRegistered() && !_user->getNick().empty() && !_user->getUser().empty())
	{
		_user->setRegistrationFlag(true);
		_user->addToSendBuffer(RPL_WELCOME(user_id(_user->getNick(), _user->getUser()), _user->getNick()));
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

void	CommandExecution::_kill()
{
	//validate permissions
	//getUser(by nickname)
	//dc user
	//notify user
	User *target = nullptr;
	if (!_command.getParams().empty())
	{
		target = _server->_findUserByNick(_command.getParams()[0]);
		std::cout << "param = " << _command.getParams()[0] << std::endl;
		if (target != nullptr)
			std::cout << "target is: " << target->getNick() << std::endl;
	}
	if (target == nullptr)
		_user->addToSendBuffer("user not found\n");
	
	// (void)target;


}

// void CommandExecution::_cap()
// {
// 	std::cout << "adding to send buffer" << std::endl;
// 	_user->addToSendBuffer("CAP * LS :\r\n");
// }}