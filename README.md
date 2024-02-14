# ft_irc - Internet Relay Chat Server
![Screenshot 2024-02-13 at 12 10 13](https://github.com/AxelBadam/ft_irc/assets/120648288/b0f99211-ec9b-422c-b6cd-29494a1d555a)

## Overview
`ft_irc` is a comprehensive educational project designed as part of the School 42 (Hive Helsinki) curriculum. It focuses on the creation and operation of an IRC (Internet Relay Chat) server, allowing real-time text-based communication over the internet (amaizing I know). 

This involves understanding and implementing various aspects of network programming in C++, including non-blocking I/O operations, handling TCP/IP (IPv4 in our case) communication, and ensuring robust error handling. The server supports key IRC features such as user authentication, nickname and username setting, joining channels, sending private messages, and channel operations (e.g., KICK, INVITE, TOPIC, MODE).

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

- gcc (GNU Compiler Collection)
- make

## Installation
```
git clone https://github.com/AxelBadam/ft_irc.git
```
```
cd ft_irc
```
```
make
```
At this point you might want to install [Irssi](https://irssi.org/) or some other IRC client in order to connect to our server.

## Running the server

```
./ircserv <port> <password>
```


![ircgif](https://github.com/AxelBadam/ft_irc/assets/120648288/e45100fa-5b14-459d-83c4-20a310f57237)


