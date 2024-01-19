/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecution.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:29:01 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/19 14:29:15 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __COMMANDEXECUTION_HPP__
#define __COMMANDEXECUTION_HPP__

#include "Server.hpp"
#include "User.hpp"
#include "Command.hpp"

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

	static void		_motd(User &user, Server &server);
	static void		_join();
	static void		_nick();
};

#endif
