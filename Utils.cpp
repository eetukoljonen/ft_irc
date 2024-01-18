/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/18 16:55:49 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/18 17:09:55 by ekoljone         ###   ########.fr       */
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

