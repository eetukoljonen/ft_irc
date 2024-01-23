/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 10:53:25 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/23 17:13:28 by ekoljone         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHANNEL_HPP__
#define __CHANNEL_HPP__

#include <map>
#include <vector>
#include "User.hpp"

class Channel
{
public:
    Channel();
    Channel(Channel const &cpy);
    ~Channel();
    Channel							&operator=(Channel const &rhs);
	bool							isOperator(std::string const &nick);
	bool							isBanned(std::string const &nick);
	bool							isKicked(std::string const &nick);
	bool							isInvateOnly();
	void							addToChannel(User *user);
	void							addToOperators(std::string const &nick);
	void							addToKickList(std::string const &nick);
	void							addToBanList(std::string const &nick);
	void							setChannelName(std::string const &name);
	std::string const				&getChannelName() const;
	void							setChannelKey(std::string const &key);
	std::string const				&getChannelkey() const;
private:
	std::map<std::string, User *>	_users;
	std::vector<std::string>		_bannedUsers;
	std::vector<std::string>		_kickedUsers;
	std::vector<std::string>		_operators;
	std::string						_topic;
	std::string						_channelName;
	std::string						_channelKey;
	bool							_invateOnly;
	int								_userLimit;
};

#endif
