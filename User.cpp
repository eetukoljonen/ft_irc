#include "User.hpp"

User::User() : _isRegistered(false) {}

User::User(User const &cpy)
{
	_nick = cpy._nick;
	_user = cpy._user;
	_ipAddr = cpy._ipAddr;
	_userInfo = cpy._userInfo;
	_userInput = cpy._userInput;
	_sendBuffer = cpy._sendBuffer;
	_isRegistered = cpy._isRegistered;
}

User::~User(){}

User &User::operator=(User const &rhs)
{
    if (this != &rhs)
    {
		_nick = rhs._nick;
		_user = rhs._user;
		_ipAddr = rhs._ipAddr;
		_userInfo = rhs._userInfo;
		_userInput = rhs._userInput;
		_sendBuffer = rhs._sendBuffer;
		_isRegistered = rhs._isRegistered;
    }
    return (*this);
}

bool const &User::isRegistered() const
{
	return (_isRegistered);
}

std::string const User::extractFromSendBuffer()
{
	std::string msg;
	if (!_sendBuffer.empty())
	{
		msg = _sendBuffer[0];
		_sendBuffer.erase(_sendBuffer.begin());
	}
	return (msg);
}

void User::addToSendBuffer(std::string const &msg)
{
	_sendBuffer.push_back(msg);
}

// t_command *User::extractCommand()
// {
// 	std::cout << "hi" << std::endl;
// 	if (!_userInput.empty())
// 		std::cout << "inside userinput = " << _userInput[0] << std::endl;
// 	if (_userInput.empty() || _userInput[0].find("\n") == std::string::npos)
// 		return (NULL);
// 	std::vector<std::string> splitted_input = split(extractInput(), ' ');
// 	t_command *command = new t_command;
// 	size_t size = splitted_input.size();
// 	for (size_t i = 0; i < size; i++)
// 	{
// 		while (i < size && splitted_input[i].empty())
// 			i++;
// 		if (command->prefix.empty()
// 			&& command->command.empty()
// 			&& command->params.empty()
// 			&& splitted_input[i].at(0) == ':')
// 			command->prefix = splitted_input[i];
// 		else if (command->command.empty()
// 				&& command->params.empty())
// 			command->command = splitted_input[i];
// 		else if (!command->command.empty()
// 				&& splitted_input[i].at(0) == ':')
// 		{
// 			std::string msg;
// 			while (i < size)
// 			{
// 				msg.append(splitted_input[i++]);
// 				if (i < size)
// 					msg.append(" ");
// 			}
// 			command->params.push_back(msg);
// 		}
// 		else
// 			command->params.push_back(splitted_input[i]);

// 	}
// 	printCommand(command);
// 	return (command);
// }

void appendInput(std::vector<std::string> &buf, std::string const &str)
{
	if (!buf.empty() && buf[0].find("\n") == std::string::npos)
		buf[0].append(str);
	else
		buf.push_back(str);
}

int User::addToInputBuffer(std::string msg)
{
	std::string cmd(msg);
	while (msg.find("\n") != std::string::npos)
	{
		cmd = msg.substr(0, msg.find("\n") + 1);
		msg.erase(0, msg.find("\n") + 1);
		appendInput(_userInput, cmd);
	}
	if (!msg.empty())
		appendInput(_userInput, msg);
	std::cout << "inside user input size: " << _userInput.size() << std::endl;
	for (size_t i = 0; i < _userInput.size(); i++)
	{
		std::cout << _userInput[i] << std::endl;
	}
	return (0);
}

std::string const User::extractInput()
{
	if (_userInput.empty())
	 	return (std::string());
	std::string const msg = _userInput[0];
	_userInput.erase(_userInput.begin());
	return (msg);
}

t_client &User::getUserInfo(){
	return _userInfo;
}

std::string const &User::getNick(){
	return _nick;
}

std::string const	&User::getUser(){
	return _user;
}

void User::setNick(std::string const &nick)
{
	_nick = nick;
}

void User::setUser(std::string const &user)
{
	_user = user;
}
