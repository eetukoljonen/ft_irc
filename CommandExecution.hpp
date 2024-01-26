/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecution.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atuliara <atuliara@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:29:01 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/26 12:58:43 by atuliara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __COMMANDEXECUTION_HPP__
#define __COMMANDEXECUTION_HPP__

#include "Server.hpp"
#include "User.hpp"
#include "Command.hpp"
#include "Utils.hpp"
#include <regex>
#include "Channel.hpp"
#include <algorithm>

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
	//join
	static void		_join();
	static void		_joinExistingChannel(Channel *channel, std::string const &key);
	static void		_joinNewChannel(std::string const &name, std::string const &key);
	static void		_joinSucces(Channel *channel);
	// nick
	static void		_nick();
	static bool 	_isValidNick();

	static void		_pass();
	static void		_userF();
	// static void		_cap();
	static void		_kill();
	static void		_kick();
	//mode
	static void		_mode();
	static void		_userMode();
	static void		_channelMode();

	//invite
	static void		_invite();
	
};


#endif
