/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecution.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:29:01 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/23 16:25:44 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __COMMANDEXECUTION_HPP__
#define __COMMANDEXECUTION_HPP__

#include "Server.hpp"
#include "User.hpp"
#include "Command.hpp"
#include "Utils.hpp"
#include <regex>

class Server;

class CommandExecution
{
public:
	static void	execute(User *user, Server *server, Command &command);

private:
	CommandExecution();
	CommandExecution(User *user, Server *server);
    CommandExecution(CommandExecution const &cpy);
    ~CommandExecution();
    CommandExecution &operator=(CommandExecution const &rhs);

	static User		*_user;
	static Server	*_server;
	static Command	_command;

	static void		_motd();
	static void		_join();
	// nick
	static void		_nick();
	static bool 	_isValidNick();

	static void		_pass();
	static void		_userF();
	// static void		_cap();
	static void		_kill();
	//mode
	static void		_mode();
	static void		_userMode();
	static void		_channelMode();
};


#endif
