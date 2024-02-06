/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 13:28:00 by ekoljone          #+#    #+#             */
/*   Updated: 2024/02/06 13:42:57 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char **argv) 
{
	if (argc != 3)
	{
		std::cerr << "try ./ircserv <port> <password>" << std::endl;
		return (1);
	}
		Server server;
		server.startServer(argv[1], argv[2]);
		return (0);
}