/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandExecution.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 15:29:01 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/18 17:05:41 by ekoljone         ###   ########.fr       */
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
	static void	execute(User &user, Server &server);
private:
	CommandExecution();
	CommandExecution(User *user, Server *server);
    CommandExecution(CommandExecution const &cpy);
    ~CommandExecution();
    CommandExecution &operator=(CommandExecution const &rhs);
};

#endif
