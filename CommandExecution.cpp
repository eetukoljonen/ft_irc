/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecution.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atuliara <atuliara@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:29:04 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/19 11:42:15 by atuliara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandExecution.hpp"

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

// void CommandExecution::execute()
// {
// 	t_command *userCommand = _user->extractCommand();
// 	if (userCommand)
// 	{
// 		if (!userCommand->command.compare("CAP"))
// 		{
// 			_getUserByFd(_pollfds[i].fd)->addToSendBuffer("CAP * LS :\r\n");
// 		}
// 		else if (!userCommand->command.compare("JOIN"))
// 		{
// 			_getUserByFd(_pollfds[i].fd)->addToSendBuffer(ERR_NOTREGISTERED(_name));
// 		}
// 		delete userCommand;
// 	}
// }



void CommandExecution::execute(User &user, Server &server)
{
	(void)server;
	std::string input = user.extractInput();
	if (input.empty())
		return ;
	Command cmd(input);
    
	std::string	Cmds[2] = {
		//"INVITE",
		"JOIN",
		//"KICK",
		// "KILL",
		// "LIST",
		// "MODE",
		// "MOTD",
		// "NAMES",
		// "NICK",
		//"NOTICE",
		//"OPER",
		//"PART",
		//"PING",
		//"PRIVMSG",
		//"QUIT",
		//"TOPIC",
		"USER",
    };
    
    int i = 0;
	std::string userCommand = cmd.getCommand();
    while (!Cmds[i].empty())
    {
        if (userCommand.compare(Cmds[i]))
            break;
        i++;
    }
   
	switch (i + 1)
	{
        case 1: std::cout << "do it" << std::endl; break;
		case 2: std::cout << "do that" << std::endl; break;
		// case 1: invite(this, client_fd, cmd_infos); break;
		// case 2: join(this, client_fd, cmd_infos); break;
		// case 3: kick(this, client_fd, cmd_infos); break;
		// case 4: kill(this, client_fd, cmd_infos); break;
		// case 5: list(this, client_fd, cmd_infos); break;
		// case 6: modeFunction(this, client_fd, cmd_infos); break;
		// case 7: motd(this, client_fd, cmd_infos); break;
		// case 8: names(this, client_fd, cmd_infos); break;
		// case 9: nick(this, client_fd, cmd_infos); break;
    	// case 10: notice(this, client_fd, cmd_infos); break;
		// case 11: oper(this, client_fd, cmd_infos); break;
		// case 12: part(this, client_fd, cmd_infos); break;
		// case 13: ping(this, client_fd, cmd_infos); break;
		// case 14: privmsg(this, client_fd, cmd_infos); break;
		// case 15: quit(this, client_fd, cmd_infos); break;
		// case 16: topic(this, client_fd, cmd_infos); break;
		// case 17: user(this, client_fd, cmd_infos); break;
		// default:
		// 	addToClientBuffer(this, client_fd, ERR_UNKNOWNCOMMAND(client->getNickname(), cmd_infos.name));
	}
}

void CommandExecution::_motd(User &user, Server &server)
{
	std::ifstream		data;

	data.open("motd");
	std::cout << "data opened" << std::endl;
	data.close();

	(void)user;
	(void)server;
}