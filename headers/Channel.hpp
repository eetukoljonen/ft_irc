/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: atuliara <atuliara@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 10:53:25 by ekoljone          #+#    #+#             */
/*   Updated: 2024/02/14 15:25:42 by atuliara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CHANNEL_HPP__
#define __CHANNEL_HPP__

# define MODE_l 1
# define MODE_t 2
# define MODE_k 4
# define MODE_i 8

#include <map>
#include <vector>
#include "User.hpp"
#include "Server.hpp"

class Channel
{
public:
    Channel();
    Channel(Channel const &cpy);
    ~Channel();
    Channel							&operator=(Channel const &rhs);
	//bools
	bool							isOperator(std::string const &nick);
	bool							isInvited(std::string const &nick);
	bool							isKicked(std::string const &nick);
	bool 							UserOnChannel(std::string const &nick);
	//setters
	void							addToChannel(User *user);
	void							addToOperators(std::string const &nick);
	void							addToKickList(std::string const &nick);
	void							addToInviteList(std::string const &nick);
	void							setChannelName(std::string const &name);
	void							setChannelKey(std::string const &key);
	void							setTopic(std::string const &topic);
	void							addChannelMode(u_int8_t const &mode);
	void							setUserLimit(int const &limit);
	void							removeFromChannel(std::string const &nick);
	void							removeOperatorPrivilages(std::string const &nick);
	void							removeChannelMode(u_int8_t const &mode);
	void							clearTopic();
	//getters
	std::string const				&getChannelName() const;
	std::string const				&getChannelkey() const;
	std::string						getNickList();
	std::string const				&getTopic() const;
	int const						&getUserLimit() const;
	int								getUserCount() const;
	User 							*getUser(std::string const &nick) const;
	std::map<std::string, User *>	&getUsersMap();
	u_int8_t const					&getChannelMode() const;
	std::string						getChannelModeString();
	//broadcast
	void							broadcastToChannel(const std::string& msg);
	void							broadcastToChannel(const std::string& msg, User *ignoredUser);
private:
	std::map<std::string, User *>	_users;
	std::vector<std::string>		_invitedUsers;
	std::vector<std::string>		_kickedUsers;
	std::vector<std::string>		_operators;
	std::string						_topic;
	std::string						_channelName;
	std::string						_channelKey;
	int								_userLimit;
	// bit set for channel modes
	u_int8_t						_modes;
};

#endif
