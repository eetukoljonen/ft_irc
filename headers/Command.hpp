/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 16:38:47 by ekoljone          #+#    #+#             */
/*   Updated: 2024/02/06 13:39:35 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <iostream>
#include <vector>
#include "Utils.hpp"

class Command
{
public:
    Command();
	Command(std::string const &input);
    Command(Command const &cpy);
    ~Command();
    Command							&operator=(Command const &rhs);
	Command							&operator=(std::string const &input);
	std::string const				&getPrefix();
	std::string const				&getCommand();
	std::vector<std::string> const	&getParams();
private:
	std::string						_prefix;
	std::string						_command;
	std::vector<std::string>		_params;

	void							_parseInput(std::string const &input);
};

#endif
