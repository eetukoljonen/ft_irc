
# define user_id(nickname, username) (":" + nickname + "!" + username + "@localhost")

# define ERR_NOTREGISTERED(servername)(":" + servername + " 451 :Register first.\r\n")
# define RPL_WELCOME(server, user_id, nickname) (":" + server + " 001 " + nickname + " :Welcome to the Internet Relay Network " + user_id + "\r\n")
# define RPL_YOURHOST(server, client, version) (":" + server + " 002 " + client + " :Your host is " + server + " (" + server + "), running version " + version + "\r\n")
# define RPL_CREATED(server, client, datetime) (":" + server + " 003 " + client + " :This server was created " + datetime + "\r\n")
# define RPL_MYINFO(server, client, version, user_modes, chan_modes, chan_param_modes) (":" + server + " 004 " + client + " " + server + " " + version + " " + user_modes + " " + chan_modes + " " + chan_param_modes + "\r\n")
# define RPL_ISUPPORT(server, client, tokens) (":" + server " 005 " + client + " " + tokens + " :are supported by this server\r\n")

# define ERR_NOMOTD(server, client) (":" + server + " 422 " + client + " :MOTD File is missing\r\n")

# define RPL_MOTDSTART(server, client) (":" + server + " 375 " + client + " :- " + server + " Message of the day - \r\n")
# define RPL_MOTD(server, client, motd_line) (":" + server  + " 372 " + client + " :" + motd_line + "\r\n")
# define RPL_ENDOFMOTD(server, client) (":" + server + " 376 " + client + " :End of /MOTD command.\r\n")

# define ERR_PASSWDMISMATCH(server, client) (":" + server + " 464 " + client + " :Password incorrect.\r\n")

# define ERR_NEEDMOREPARAMS(server, client, command) (":" + server + " 461 " + client + " " + command + " :Not enough parameters.\r\n")

//nick

#define ERR_NONICKNAMEGIVEN(server) (":" + server + " 431 :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(server, nick) (":" + server + " 432 " + nick + " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE(server, nick) (":" + server + " 433 " + nick + " :Nickname is already in use\r\n")
#define ERR_NICKCOLLISION(server, nick) (":" + server + " 436 " + nick + " :Nickname collision\r\n")
#define ERR_RESTRICTED(server, nick) (":" + server + " 484 " + nick + " :Your connection is restricted and cannot use this nickname\r\n")

#define RPL_KILL(user_id, killed, comment) (user_id + " KILL " + killed + " " + comment + "\r\n")
#define ERR_NOSUCHNICK(servername, client, nickname) (":" + servername + " 401 " + client + " " + nickname + " :No such nick/channel.\r\n")

//user

# define RPL_UMODEIS(servername, client, user_modes) (":" + servername + " 221 " + client + " :" + user_modes + "\r\n")
# define ERR_UMODEUNKNOWNFLAG(servername, client, unknown_mode) (":" + servername + " * 501 " + client + " :Unknown mode flag '" + unknown_mode + "'\r\n")

//join
# define ERR_INVITEONLYCHAN(servername, client, channel) (":" + servername + " 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")

// ERR_BADCHANNELKEY (475)
# define ERR_BADCHANNELKEY(servername, client, channel) (":" + servername + " 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")

// ERR_CHANNELISFULL (471)
# define ERR_CHANNELISFULL(servername, client, channel) (":" + servername + " 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")

// ERR_BADCHANMASK (476)
# define ERR_BADCHANMASK(servername, client, channel) (":" + servername + " 476 " + client + " " + channel + " :Bad Channel Mask\r\n")

// ERR_NOSUCHCHANNEL (403)
# define ERR_NOSUCHCHANNEL(servername, client, channel) (":" + servername + " 403 " + client + " " + channel + " :No such channel\r\n")

// ERR_TOOMANYCHANNELS (405)
# define ERR_TOOMANYCHANNELS(servername, client, channel) (":" + servername + " 405 " + client + " " + channel + " :You have joined too many channels\r\n")

// ERR_TOOMANYTARGETS (407)
# define ERR_TOOMANYTARGETS(servername, client, target) (":" + servername + " 407 " + client + " " + target + " :Too many recipients\r\n")

// ERR_UNAVAILRESOURCE (437)
# define ERR_UNAVAILRESOURCE(servername, client, target) (":" + servername + " 437 " + client + " " + target + " :Cannot change nickname while services are running\r\n")

// RPL_TOPIC (332)
# define RPL_TOPIC(servername, client, channel, topic) (":" + servername + " 332 " + client + " " + channel + " :" + topic + "\r\n")