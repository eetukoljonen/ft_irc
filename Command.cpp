/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 16:38:54 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/19 11:39:49 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"

Command::Command(){}

Command::Command(std::string const &input)
{
	_parseInput(input);
}

Command::Command(Command const &cpy)
{
	_prefix = cpy._prefix;
	_command = cpy._command;
	_params = cpy._params;
}

Command::~Command(){}

Command &Command::operator=(Command const &rhs)
{
    if (this != &rhs)
    {
		_prefix = rhs._prefix;
		_command = rhs._command;
		_params = rhs._params;
    }
    return (*this);
}

Command &Command::operator=(std::string const &input)
{
    _prefix.clear();
	_command.clear();
	_params.clear();
    _parseInput(input);
    return (*this);
}

void Command::_parseInput(std::string const &input)
{
	if (input.empty() || input.find("\n") == std::string::npos)
		return;
	std::vector<std::string> splitted_input = split(input, ' ');
	size_t size = splitted_input.size();
	for (size_t i = 0; i < size; i++)
	{
		while (i < size && splitted_input[i].empty())
			i++;
		if (_prefix.empty()
			&& _command.empty()
			&& _params.empty()
			&& splitted_input[i].at(0) == ':')
			_prefix = splitted_input[i];
		else if (_command.empty()
				&& _params.empty())
				_command = splitted_input[i];
		else if (!_command.empty()
				&& splitted_input[i].at(0) == ':')
		{
			std::string msg;
			while (i < size)
			{
				msg.append(splitted_input[i++]);
				if (i < size)
					msg.append(" ");
			}
			_params.push_back(msg);
		}
		else
			_params.push_back(splitted_input[i]);

	}
}

std::string const &Command::getPrefix()
{
	return (_prefix);
}

std::string const &Command::getCommand()
{
	return (_command);
}

std::vector<std::string> const &Command::getParams()
{
	return (_params);
}
