/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ekoljone <ekoljone@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 10:53:25 by ekoljone          #+#    #+#             */
/*   Updated: 2024/01/30 16:17:24 by ekoljone         ###   ########.fr       */
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

class Channel
{
public:
    Channel();
    Channel(Channel const &cpy);
    ~Channel();
    Channel							&operator=(Channel const &rhs);
	bool							isOperator(std::string const &nick);
	bool							isInvited(std::string const &nick);
	bool							isKicked(std::string const &nick);
	bool							isInviteOnly();
	void							addToChannel(User *user);
	void							addToOperators(std::string const &nick);
	void							addToKickList(std::string const &nick);
	void							addToInviteList(std::string const &nick);
	void							setChannelName(std::string const &name);
	std::string const				&getChannelName() const;
	void							setChannelKey(std::string const &key);
	std::string const				&getChannelkey() const;
	std::string const				&getNickList() const;
	std::string const				&getTopic() const;
	int const						&getUserLimit() const;
	int								getUserCount() const;
	void							setInviteOnly(bool const &flag);

	bool 							UserOnChannel(std::string const &nick);
	User 							*getUser(std::string const &nick) const;
	void							broadcastToChannel(const std::string& msg);
	std::map<std::string, User *>	&getUsersMap();
	void							removeFromChannel(std::string const &nick);
	void							removeOperatorPrivilages(std::string const &nick);
	void							addChannelMode(u_int8_t const &mode);
	void							removeChannelMode(u_int8_t const &mode);
	u_int8_t const					&getChannelMode() const;
	void							setUserLimit(int const &limit);
	std::string						getChannelModeString();
private:
	std::map<std::string, User *>	_users;
	std::vector<std::string>		_invitedUsers;
	std::vector<std::string>		_kickedUsers;
	std::vector<std::string>		_operators;
	std::string						_topic;
	std::string						_channelName;
	std::string						_channelKey;
	std::string						_nickList;
	bool							_inviteOnly;
	int								_userLimit;
	// bit set for channel modes
	u_int8_t						_modes;
};

#endif
