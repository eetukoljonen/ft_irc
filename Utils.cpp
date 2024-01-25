/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 16:55:49 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/25 11:39:21 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

std::vector<std::string> split(std::string const &str, char const &delimeter)
{
	std::istringstream			iss(str);
	std::vector<std::string>	tokens;
	std::string					token;

	while (std::getline(iss, token, delimeter))
		tokens.push_back(token);
	for (size_t i = 0; i < tokens.size(); i++)
	{
		std::cout << tokens[i] << std::endl;
	}
	return (tokens);
}

std::string truncateLFCR(std::string const &str)
{
	std::string result = str.substr(0, str.find("\n"));
	result = result.substr(0, str.find("\r"));
	return (result);
}

bool checkIrcPattern(std::string const &str)
{
    // Regular expression for a valid IRC nickname
    std::regex ircPattern("^[A-Za-z[\\]^_`][A-Za-z0-9[\\]^_`]*$");
    // Check if the nickname matches the pattern
    return (std::regex_match(str, ircPattern));
}