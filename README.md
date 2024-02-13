# ft_irc - Internet Relay Chat Server

## Overview
`ft_irc` is a comprehensive educational project designed as part of the School 42 (Hive Helsinki) curriculum. It focuses on the creation and operation of an IRC (Internet Relay Chat) server, allowing real-time text-based communication over the internet (amaizing I know).  This involves understanding and implementing various aspects of network programming in C++, including non-blocking I/O operations, handling TCP/IP (IPv4 in our case) communication, and ensuring robust error handling. The server supports key IRC features such as user authentication, nickname and username setting, joining channels, sending private messages, and channel operations (e.g., KICK, INVITE, TOPIC, MODE).

## Commands

| Command  | Description                                       |
|----------|---------------------------------------------------|
| NICK     | Change your nickname.                             |
| USER     | Set your username.                                |
| PASS     | Authenticate with the server using a password.    |
| JOIN     | Join a chat channel.                              |
| PART     | Leave a chat channel.                             |
| PRIVMSG  | Send a private message to a user or channel.      |
| MODE     | Adjust user or channel modes.                     |
| KICK     | Eject a user from a channel.                      |
| TOPIC    | Set or change a channel's topic.                  |
| INVITE   | Invite a user to a channel.                       |
| QUIT     | Disconnect from the IRC server.                   |
| MOTD     | Display the message of the day.                   |
| PING     | Test the server's response.                       |
| PONG     | Respond to a PING request.                        |

## Prerequisites

Before starting on the `ft_irc` project, you should have the following installed on your development environment:

- gcc (GNU Compiler Collection)
- make

## Installation
``` git clone 


